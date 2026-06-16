#include<stdio.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<iostream>
#include<thread>
#include<vector>
using namespace std;
using namespace std::chrono;
void processData(int source_fd,int target_fd,size_t chunkSize,off_t offset)
{

	long pageSize=sysconf(_SC_PAGE_SIZE);

	off_t alignedOffSet=(offset/pageSize) * pageSize;
	size_t extra=offset-alignedOffSet;
	size_t mappedSize=chunkSize+extra;

char *source_ptr=(char *)mmap(NULL,mappedSize,PROT_READ,MAP_PRIVATE,source_fd,alignedOffSet);

if(source_ptr==MAP_FAILED)
{
close(source_fd);
cout<<"Unable to map source file "<<endl;
//use some fallback technique
return;
}

char *target_ptr=(char *)mmap(NULL,mappedSize,PROT_READ | PROT_WRITE,MAP_SHARED,target_fd,alignedOffSet);

if(target_ptr==MAP_FAILED)
{
//cout<<"Unable to map Process Data .2"<<endl;
munmap(source_ptr,chunkSize);
//close(source_fd);
//close(target_fd);
return ;
}

memcpy(target_ptr+extra,source_ptr+extra,chunkSize);
//cout<<"File "<<target<<" created "<<endl;

msync(target_ptr+extra,chunkSize,MS_SYNC);
munmap(source_ptr,mappedSize);
munmap(target_ptr,mappedSize);

}

int main()
{

	auto start=high_resolution_clock::now();

	unsigned int numThreads=thread::hardware_concurrency();
//	cout<<"Number of thread is "<<numThreads<<endl;
	vector<thread> threads;
	string source="test.data";
	string target="abc.abc";
	struct stat s;
	if(stat(source.c_str(),&s)<0)
	{
		cout<<"Unable to determine size of "<<source<<endl;
		return 1;
	}
	size_t source_file_size=s.st_size;
	int source_fd;
	source_fd=open(source.c_str(),O_RDONLY);
	if(source_fd<0)
	{

		cout<<"Unable to open "<<source<<endl;
		return 1;
	}


long pageSize=sysconf(_SC_PAGE_SIZE);


int target_fd;
target_fd=open(target.c_str(),O_RDWR | O_CREAT | O_TRUNC , 0666);
if(target_fd<0)
{
//munmap(source_ptr,source_file_size);	
close(source_fd);
return 1;
}

if(ftruncate(target_fd,source_file_size)<0)
{
	cout<<"Unable to setup size of Main function"<<endl;
	//munmap(source_ptr,source_file_size);
	close(source_fd);
	close(target_fd);
	return 1;
}



off_t offset=0;
size_t chunkSize=source_file_size/numThreads;

//cout<<"chunkSize is "<<chunkSize<<endl;

//cout<<"File size is "<<source_file_size<<endl;

chunkSize=(chunkSize/pageSize) * pageSize;

if(chunkSize==0)
{
chunkSize=source_file_size/numThreads;
}

for(int i=0;i<numThreads;i++)
{
	size_t currentChunk=chunkSize;
	if(i==numThreads-1) currentChunk=source_file_size-offset;

//cout<<"current chunk is "<<currentChunk<<endl;

	threads.emplace_back(processData,source_fd,target_fd,currentChunk,offset);
//	cout<<"Offset is "<<offset<<endl;
	offset+=currentChunk;

}



for(thread &t:threads)
{
	t.join();
}



close(source_fd);
close(target_fd);

auto end=high_resolution_clock::now();
auto d=duration_cast<milliseconds>(end-start);
cout<<d.count()<<" milliseconds "<<endl;

return 0;


}
