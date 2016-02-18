
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <OpenCL/opencl.h>
#include <time.h>
// Simple compute kernel which computes the square of an input array
//
const char *KernelSource = "\n" \
"__kernel void Parallel_Search(                                                       \n" \
"   __global const char *a_buffer,                                              \n" \
"   __global const char *b_buffer,                                             \n" \
"   global int *result_buffer,                                       \n" \
"   global int *m_buffer)                                           \n" \
"{   int j=0;                                                                   \n" \
"   int i = get_global_id(0);                                           \n" \
"   while(b_buffer[j])                                           \n" \
"   if(a_buffer[i * (*m_buffer) + j]!=b_buffer[j])                                                \n" \
"      { result_buffer[i]=0;                               \n" \
"       break;}                               \n" \
"      if(!b_buffer[j])                               \n" \
"       result_buffer[i]=1;                              \n" \
"}                                                                      \n" \
"\n";

////////////////////////////////////////////////////////////////////////////////


struct suffix
{
    int index;
    char *suff;
};
int cmpfunc(const void *a, const void *b)
{
    return (strcmp(((struct suffix *)a)->suff ,((struct suffix *)b)->suff));
    
}
int *buildSuffixArray(struct suffix suffixes[],char *txt, int n)
{
    
    for (int i = 0; i < n; i++)
    {
        suffixes[i].index = i;
        suffixes[i].suff = (txt+i);
    }
    
    qsort(suffixes, n,sizeof(struct suffix), cmpfunc);
    int *suffixArr = (int *)malloc(sizeof(int)*n);
    for (int i = 0; i < n; i++)
        suffixArr[i] = suffixes[i].index;
    return  suffixArr;
    
}
void buildsuffix(char *suffix[], struct suffix suffixes[],int n,int m)
{  int i;
    
    for(i=0;i<n;i++)
    {
        suffix[i]=suffixes[i].suff;
        //printf("%s\n",suffix[i]);
    }
}

void printArr(struct suffix suffixes[], int n)
{
    for(int i = 0; i < n; i++)
    {printf("%d ",suffixes[i].index);
        printf("%s \n",suffixes[i].suff);
    }
    printf("\n");
}
int main(int argc, char** argv)
{
    int err;                            // error code returned from api calls
    clock_t start,end, total;
    char txt[] = "manitbhopalcsedepartment";  //text
    char pat[] = "cse"; //pattern
    
    // Build suffix array
    int n = strlen(txt);
    struct suffix suffixes[n];
    int *suffixArr = buildSuffixArray(suffixes,txt, n);
    printf("Following is suffix array for %s \n",txt);
    printArr(suffixes, n);
    int *suffArr = buildSuffixArray(suffixes,txt, n);
    //search(pat, txt, suffArr, n);
    int m = strlen(pat);
    char suffix[n*m];
    int i,j;
  //  buildsuffix(suffix,suffixes,n,m);
    start=clock();
       int k=0;
    for(i=0;i<n;i++)
    {
        for(j=0;j<m;j++)
        {
            if(!suffixes[i].suff[j])
            {k--;break;}
            else
                suffix[k*m+j]=suffixes[i].suff[j];
            }
            k++;
        //suffix[i][j]='\0';
        //printf("\n%s",suffix[i]);
        //printf("\n");
    
    }
    cl_char a[k*m];
  
    for(i=0;i<k;i++)
    {         for(j=0;j<m;j++)
    {a[i*m+j]=suffix[i*m+j];
            printf("%c",a[i*m+j]);
        }printf("\n");
    }
    
    // Do simple binary search for the pat in txt using the
    // built suffix array
    // clock_t start = clock();
    
    
    //Simple ENDS
    
   // int l = 0, r = n-1;
    size_t global;                      // global domain size for our calculation
    size_t local;                       // local domain size for our calculation
    
    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel
    //cl_char a[m];
    cl_char b[m];
    memcpy(b, pat, m);
    b[m] = '\0';
 
        int gpu = 1;
        err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to create a device group!\n");
            return EXIT_FAILURE;
        }
        
        // Create a compute context
        //
        context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
        if (!context)
        {
            printf("Error: Failed to create a compute context!\n");
            return EXIT_FAILURE;
        }
        
        // Create a command commands
        //
        commands = clCreateCommandQueue(context, device_id, 0, &err);
        if (!commands)
        {
            printf("Error: Failed to create a command commands!\n");
            return EXIT_FAILURE;
        }
        
        // Create the compute program from the source buffer
        //
        program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
        if (!program)
        {
            printf("Error: Failed to create compute program!\n");
            return EXIT_FAILURE;
        }
        
        // Build the program executable
        //
        err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
        if (err != CL_SUCCESS)
        {
            size_t len;
            char buffer[2048];
            
            printf("Error: Failed to build program executable!\n");
            clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
            printf("%s\n", buffer);
            exit(1);
        }
        
        // Create the compute kernel in the program we wish to run
        //
        kernel = clCreateKernel(program, "Parallel_Search", &err);
        if (!kernel || err != CL_SUCCESS)
        {
            printf("Error: Failed to create compute kernel!\n");
            exit(1);
        }
    cl_int m1=m;
        
    
        //printf("%s\n",a);
        cl_mem a_buffer = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_char) * k*m, NULL, NULL);
        
        cl_mem b_buffer = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_char) * m, NULL, NULL);
        cl_mem result_buffer = clCreateBuffer(context,  CL_MEM_WRITE_ONLY,  sizeof(cl_int) * k, NULL, NULL);
    cl_mem m_buffer=clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(cl_int), NULL, NULL);

    
        // Write our data set into the input array in device memory
        //
        err = clEnqueueWriteBuffer(commands, a_buffer, CL_TRUE, 0, sizeof(char) * k*m,a, 0, NULL, NULL);
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to write to source array!\n");
            exit(1);
        }
        err = clEnqueueWriteBuffer(commands, b_buffer, CL_TRUE, 0, sizeof(char) * m, b, 0, NULL, NULL);
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to write to source array1!\n");
            exit(1);
        }
    
err = clEnqueueWriteBuffer(commands, m_buffer, CL_TRUE, 0, sizeof(int), &m1, 0, NULL, NULL);
if (err != CL_SUCCESS)
{
    printf("Error: Failed to write to source array1!\n");
    exit(1);
}
        // Set the arguments to our compute kernel
        //
        err = 0;
        err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_buffer);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_buffer);
        err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &result_buffer);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &m_buffer);
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to set kernel arguments! %d\n", err);
            exit(1);
        }
        
        // Get the maximum work group size for executing the kernel on the device
        //
        err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to retrieve kernel work group info! %d\n", err);
            exit(1);
        }
        
        // Execute the kernel over the entire range of our 1d input data set
        // using the maximum number of work group items for this device
        //
        global =16;
        err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global,NULL, 0, NULL, NULL);
        if (err)
        {
            printf("Error: Failed to execute kernel!\n");
            return EXIT_FAILURE;
        }
        
        // Wait for the command commands to get serviced before reading back results
        //
        clFinish(commands);//synchronzation...
        
        // Read back the results from the device to verify the output
        //
        int *result = (int *) malloc(m*sizeof(int));
        err = clEnqueueReadBuffer( commands, result_buffer, CL_TRUE, 0, sizeof(int) * k, result, 0, NULL, NULL );
        if (err != CL_SUCCESS)
        {
            printf("Error: Failed to read output array! %d\n", err);
            exit(1);
        }
//        int res=0;
        for( i=0;i<k;i++)
        { //printf("%d",result[i]);
            if(result[i]==1)
            {
                printf( "Pattern found ");
                break;
            }
        };
    if(i==k)
        printf("Pattern not found ...");

        //  printf("%s\n",a);
       /* if (res == 0)
        {
            printf( "Pattern found at index %d ",suffArr[mid]);
            
            return 0;
        }
        if (res < 0) r = mid - 1;
        else l = mid + 1;
        */
        clReleaseMemObject(a_buffer);
        clReleaseMemObject(b_buffer);
        clReleaseProgram(program);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(commands);
        clReleaseContext(context);
    end=clock();
    total=(double)(end-start)/CLOCKS_PER_SEC;
    printf("\nTime taken: %5.15f",total);
       return 0;
}


