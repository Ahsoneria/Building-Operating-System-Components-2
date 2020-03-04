//Ayush Hitesh Soneria
//170192
//OS assignment 2, pipe.c
#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>
/***********************************************************************
 * Use this function to allocate pipe info && Don't Modify below function
 ***********************************************************************/
struct pipe_info* alloc_pipe_info()
{
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);
    pipe ->pipe_buff = buffer;
    return pipe;
}


void free_pipe_info(struct pipe_info *p_info)
{
    if(p_info)
    {
        os_page_free(OS_DS_REG ,p_info->pipe_buff);
        os_page_free(OS_DS_REG ,p_info);
    }
}
/*************************************************************************/
/*************************************************************************/


int pipe_read(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Read the contect from buff (pipe_info -> pipe_buff) and write to the buff(argument 2);
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    if(!filep)
        return -EINVAL;
    if(!(filep->mode & O_READ))
        return -EACCES;
    int k=filep->pipe->read_pos;//keep track of read position
    int i=0;
    while(i<count)
    {
        if(filep->pipe->buffer_offset==0)//nothing left to read in pipe
            return -EOTHERS;
        buff[i] = filep->pipe->pipe_buff[k];
        i++;
        k++;
        filep->pipe->buffer_offset--;
    }
    filep->pipe->read_pos = k;
    return i;
}


int pipe_write(struct file *filep, char *buff, u32 count)
{
    /**
    *  TODO:: Implementation of Pipe Read
    *  Write the contect from   the buff(argument 2);  and write to buff(pipe_info -> pipe_buff)
    *  Validate size of buff, the mode of pipe (pipe_info->mode),etc
    *  Incase of Error return valid Error code 
    */
    if(!filep)
        return -EINVAL;
    if(!(filep->mode & O_WRITE))
        return -EACCES;
    int i=0;
    int k=filep->pipe->write_pos;//keep track of write position
    while((i<count))
    {
        if(filep->pipe->buffer_offset == PIPE_MAX_SIZE)//pipe size limit exceeded
            return -EOTHERS;
        filep->pipe->pipe_buff[k] = buff[i];
        i++;
        k++;
        filep->pipe->buffer_offset++;
    }
    filep->pipe->write_pos = k;
    return i;
}

int create_pipe(struct exec_context *current, int *fd)
{
    /**
    *  TODO:: Implementation of Pipe Create
    *  Create file struct by invoking the alloc_file() function, 
    *  Create pipe_info struct by invoking the alloc_pipe_info() function
    *  fill the valid file descriptor in *fd param
    *  Incase of Error return valid Error code 
    */
    fd[0] = 0;
    while(current->files[fd[0]])
    {
        fd[0]++;
        if(fd[0] == MAX_OPEN_FILES)
            return -EINVAL;
    }
    fd[1] = fd[0]+1;
    if(fd[1] == MAX_OPEN_FILES)
            return -EINVAL;
    while(current->files[fd[1]])
    {
        fd[1]++;
        if(fd[1] == MAX_OPEN_FILES)
            return -EINVAL;
    }
    struct file* f1 = alloc_file();
    if(!f1)
        return -ENOMEM;
    struct file* f2 = alloc_file();
    if(!f2)
        return -ENOMEM;
    struct pipe_info* p = alloc_pipe_info();
    if(!p)
        return -ENOMEM;
    p->read_pos = 0;
    p->write_pos = 0;
    p->buffer_offset = 0;
    //making two file objects for read and write ends of pipe p 
    f1->pipe = p;
    f1->type = PIPE;
    f1->inode = NULL;
    f1->mode = O_READ;
    f1->ref_count = 1;
    f1->pipe->is_ropen = 1;
    f1->pipe->is_wopen = 0;
    f2->pipe = p;
    f2->type = PIPE;
    f2->inode = NULL;
    f2->mode = O_WRITE;
    f2->ref_count = 1;
    f2->pipe->is_ropen = 0;
    f2->pipe->is_wopen = 1;
    f1->fops->read = pipe_read;
    f2->fops->write = pipe_write;
    f1->fops->close = generic_close;
    f2->fops->close = generic_close;
    current->files[fd[0]] = f1;
    current->files[fd[1]] = f2;
}

