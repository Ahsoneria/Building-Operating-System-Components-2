//Ayush Hitesh Soneria
//170192
//OS assignment 2, file.c
#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>
#include<pipe.h>


/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/
void free_file_object(struct file *filep)
{
	if(filep)
	{
	   os_page_free(OS_DS_REG ,filep);
	   stats->file_objects--;
	}
}

struct file *alloc_file()
{
  
  struct file *file = (struct file *) os_page_alloc(OS_DS_REG); 
  file->fops = (struct fileops *) (file + sizeof(struct file)); 
  bzero((char *)file->fops, sizeof(struct fileops));
  stats->file_objects++;
  return file; 
}

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
  kbd_read(buff);
  return 1;
}

static int do_write_console(struct file* filep, char * buff, u32 count)
{
  struct exec_context *current = get_current_ctx();
  return do_write(current, (u64)buff, (u64)count);
}

struct file *create_standard_IO(int type)
{
  struct file *filep = alloc_file();
  filep->type = type;
  if(type == STDIN)
	 filep->mode = O_READ;
  else
	  filep->mode = O_WRITE;
  if(type == STDIN){
		filep->fops->read = do_read_kbd;
  }else{
		filep->fops->write = do_write_console;
  }
  filep->fops->close = generic_close;
  filep->ref_count = 1;
  return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
   int fd = type;
   struct file *filep = ctx->files[type];
   if(!filep){
		filep = create_standard_IO(type);
   }else{
		 filep->ref_count++;
		 fd = 3;
		 while(ctx->files[fd])
			 fd++; 
   }
   ctx->files[fd] = filep;
   return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/



void do_file_fork(struct exec_context *child)
{
   /*TODO the child fds are a copy of the parent. Adjust the refcount*/
	// if(!ctx)
	// 	return;
	for(int i=0;i<MAX_OPEN_FILES;i++)
	{
		if(child->files[i])
			child->files[i]->ref_count++;
	}
}

void do_file_exit(struct exec_context *ctx)
{
   /*TODO the process is exiting. Adjust the ref_count
	 of files*/
	// if(!ctx)
	// 	return;
	for(int i=0;i<MAX_OPEN_FILES;i++)
	{
		generic_close(ctx->files[i]);
	}
}

long generic_close(struct file *filep)
{
  /** TODO Implementation of close (pipe, file) based on the type 
   * Adjust the ref_count, free file object
   * Incase of Error return valid Error code 
   */
	if(!filep)
		return -EINVAL;
	if(filep->type == REGULAR)
	{
		if(filep->ref_count==0)
			return -EINVAL;
		filep->ref_count--;
		if(filep->ref_count==0)
			free_file_object(filep);
		return filep->ref_count;
	}
	else if(filep->type == PIPE)
	{
		if(filep->ref_count==0)
			return -EINVAL;
		filep->ref_count--;
		if(filep->ref_count==0)
		{
			if(filep->pipe->is_ropen>0)
				filep->pipe->is_ropen = 0;
			else if(filep->pipe->is_wopen>0)
				filep->pipe->is_wopen = 0;
			if(filep->pipe->is_ropen==0 && filep->pipe->is_wopen==0)
				free_pipe_info(filep->pipe);
			free_file_object(filep);
		}
		return filep->ref_count;
	}
	//return -EINVAL;//else file type wrong
}

static int do_read_regular(struct file *filep, char * buff, u32 count)
{
   /** TODO Implementation of File Read, 
	*  You should be reading the content from File using file system read function call and fill the buf
	*  Validate the permission, file existence, Max length etc
	*  Incase of Error return valid Error code 
	* */
	if(!filep)
		return -EINVAL;
	struct inode* i = filep->inode;
	if(i==NULL)
		return -EINVAL;
	if(!(filep->mode & O_READ))
		return -EACCES; 
	int rd = flat_read(i,buff,count,&(filep->offp));
	if(rd<0)
		return -EOTHERS;
	filep->offp += rd;//not necessarily count, could be remain len from flat_read.
	if(filep->offp > FILE_SIZE)
		return -EOTHERS;
	return rd;
}


static int do_write_regular(struct file *filep, char * buff, u32 count)
{
	/** TODO Implementation of File write, 
	*   You should be writing the content from buff to File by using File system write function
	*   Validate the permission, file existence, Max length etc
	*   Incase of Error return valid Error code 
	* */
	if(!filep)
		return -EINVAL;
	struct inode* i = filep->inode;
	if(i==NULL)
		return -EINVAL;
	if(!(filep->mode & O_WRITE))
		return -EACCES;
	int wd = flat_write(i,buff,count,&(filep->offp));//error returns -1
	if(wd<0)
		return -EOTHERS;
	filep->offp += wd;
	if(filep->offp > FILE_SIZE)
		return -EOTHERS;
	return wd;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
	/** TODO Implementation of lseek 
	*   Set, Adjust the ofset based on the whence
	*   Incase of Error return valid Error code 
	* */
	if(!filep)
		return -EINVAL;
	int size = filep->inode->file_size;
	if(size > FILE_SIZE)
		return -EOTHERS;
	if(whence == SEEK_SET)
	{
		filep->offp = offset;
		if(filep->offp > size)
		{
			return -EINVAL;
		}
		return filep->offp;
	}
	else if(whence == SEEK_CUR)
	{
		filep->offp += offset;
		if(filep->offp > size)
		{
			return -EINVAL;
		}
		return filep->offp;
	}
	else if(whence == SEEK_END)
	{
		filep->offp = size + offset;
		if(filep->offp > size)
		{
			return -EINVAL;
		}
		return filep->offp;
	}
	return -EINVAL;
}

extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{ 
  /**  TODO Implementation of file open, 
	*  You should be creating file(use the alloc_file function to creat file), 
	*  To create or Get inode use File system function calls, 
	*  Handle mode and flags 
	*  Validate file existence, Max File count is 32, Max Size is 4KB, etc
	*  Incase of Error return valid Error code 
	* */
	struct inode* new = lookup_inode(filename);
	if(new==NULL)
	{
		if(flags & O_CREAT)//create new file object
		{
			new = create_inode(filename,mode);
			int fd = 0;
		    while(ctx->files[fd])
		    {
		        fd++;
		        if(fd == MAX_OPEN_FILES)
		            return -EINVAL;
		    }
			struct file* f = alloc_file();
			if(!f)
		        return -ENOMEM;
			f->type = REGULAR;
			f->mode = flags&(~(O_CREAT));
			f->inode = new;
			f->offp = 0;
			f->ref_count = 1;
			f->fops->read = do_read_regular;
			f->fops->write = do_write_regular;
			f->fops->close = generic_close; 
			f->fops->lseek = do_lseek_regular;
			f->pipe = NULL;
			ctx->files[fd] = f;
			return fd;
		}
		else
		{
			return -EINVAL;
		}
	}
	else
	{
		if(flags & O_CREAT)//give error as file is already created
		{
			return -EOTHERS;
		}
		if(!(new->mode & flags))//e.g. file is created in read mode and later opened in write mode should result in error
		{
			return -EACCES;
		}
		//else make new fd
		int fd = 0;
	    while(ctx->files[fd])
	    {
	        fd++;
	        if(fd == MAX_OPEN_FILES)
	            return -EINVAL;
	    }
		struct file* f = alloc_file();
		if(!f)
	        return -ENOMEM;
		f->type = REGULAR;
		f->mode = flags;
		f->inode = new;
		f->offp = 0;
		f->ref_count = 1;
		f->fops->read = do_read_regular;
		f->fops->write = do_write_regular;
		f->fops->close = generic_close; 
		f->fops->lseek = do_lseek_regular;
		f->pipe = NULL;
		ctx->files[fd] = f;
		return fd;
	}

}

int fd_dup(struct exec_context *current, int oldfd)
{
	 /** TODO Implementation of dup 
	  *  Read the man page of dup and implement accordingly 
	  *  return the file descriptor,
	  *  Incase of Error return valid Error code 
	  * */
	if(oldfd >= MAX_OPEN_FILES)
		return -EINVAL;
	if(!(current->files[oldfd]))
		return -EINVAL;
	int fd = 0;
    while(current->files[fd])
    {
        fd++;
        if(fd == MAX_OPEN_FILES)
            return -EINVAL;
    }
	current->files[fd] = current->files[oldfd];
	current->files[oldfd]->ref_count++;
	return fd;
}


int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
  /** TODO Implementation of the dup2 
	*  Read the man page of dup2 and implement accordingly 
	*  return the file descriptor,
	*  Incase of Error return valid Error code 
	* */
	if(oldfd >= MAX_OPEN_FILES)
		return -EINVAL;
	if(newfd >= MAX_OPEN_FILES)
		return -EINVAL;
	if(!(current->files[oldfd]))
		return -EINVAL;
	if((current->files[newfd]) && (newfd!=oldfd))
	{
		generic_close(current->files[newfd]);
	}
	if(newfd!=oldfd)//if equal then dup2 does nothing
	{
		current->files[newfd] = current->files[oldfd];
		current->files[oldfd]->ref_count++;
	}
	return newfd;
}
