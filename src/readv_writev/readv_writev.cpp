#include<sys/types.h>
#include<sys/uio.h>
#include<unistd.h>
#include<iostream>
#include<fcntl.h>
#include<sys/mman.h> 
#include<chrono>
#include<string>
#include<cstring>
#include<sys/stat.h>
using namespace std;
using namespace std::chrono;

int main()
{

auto start=high_resolution_clock::now();
ssize_t bytes_written;
char buffer1[4096];
char buffer2[4096];
char buffer3[4096];
char buffer4[4096];

struct stat s;

int sfd;
int tfd;

stat("test.data",&s);

sfd=open("test.data",O_RDONLY);
if(!sfd)
{
	cout<<"test.data file doesn't exist "<<endl;
	close(sfd);
	return 0;
}
tfd=open("dummy4.data",O_RDWR | O_CREAT | O_TRUNC,0666);

if(!tfd)
{
	cout<<"Dummy3.data file is not created "<<endl;
close(tfd);
close(sfd);
	return 0;

}


size_t file_size=s.st_size;

//cout<<"File size is "<<file_size<<endl;

if(ftruncate(tfd,file_size)>0)
{
	cout<<"File size is 0 for target file "<<endl;
	close(sfd);
	close(tfd);
	return 0;
}

ssize_t bytes_read;
ssize_t total_bytes_read;
int iocnt;
struct iovec iov[4];

iov[0].iov_base=buffer1;
iov[0].iov_len=sizeof(buffer1);
iov[1].iov_base=buffer2;
iov[1].iov_len=sizeof(buffer2);

iov[2].iov_base=buffer3;
iov[2].iov_len=sizeof(buffer3);



iov[3].iov_base=buffer4;
iov[3].iov_len=sizeof(buffer4);

struct iovec write_iov[4];

int ivocnt=4;

memcpy(write_iov,iov,sizeof(iov));

size_t total_buffer_size=(4096*4);

while((bytes_read=readv(sfd,iov,ivocnt))>0)
{

if(bytes_read==total_buffer_size)
{
writev(tfd,iov,ivocnt);
}
else 
{
size_t remaining=bytes_read;
int write_ivocnt=ivocnt;
for(int i=0;i<write_ivocnt;i++)
{
	if(remaining>=iov[i].iov_len)
	{
		write_iov[i].iov_len=iov[i].iov_len;
		remaining-=iov[i].iov_len;
	}else
	{
		write_iov[i].iov_len=remaining;
		write_ivocnt=i+1;
		break;
	}

}
writev(tfd,write_iov,write_ivocnt);

}

}


close(sfd);
close(tfd);


auto end=high_resolution_clock::now();

auto d=duration_cast<milliseconds>(end-start);
cout<<"time taken for copying file "<<d.count()<<" milliseconds "<<endl;




	return 0;
}
