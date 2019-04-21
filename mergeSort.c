#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void merge(int *a,int i,int mid, int j){
	int x=i,y=mid+1;
	int arr[j-i+1],k=0;
	while(x<=mid && y<=j){
		if(a[x]<=a[y]){
			arr[k++]=a[x++];
		}else{
			arr[k++]=a[y++];
		}
	}
	if(x<=mid){
		while(x<=mid){
			arr[k++]=a[x++];
		}
	}
	if(y<=j){
		while(y<=j){
			arr[k++]=a[y++];
		}
	}
	for(k=i;k<=j;k++){
		a[k]=arr[k-i];
	}
	return;
}

void mergeSort(int *a,int count, int i, int j){
	// if(i>=count){
	// 	return;
	// }
	// if(j>=count){
	// 	j=count-1;
	// }
	if(i>=j){
		return;
	}
	int mid=(i+j)/2;
	mergeSort(a,count,i,mid);
	mergeSort(a,count,mid+1,j);
	merge(a,i,mid,j);
	return;
}

int min(int x,int y){
	return x<=y ? x:y;
}

int main(int argc, char* argv[]){
	int size,my_rank;
	//Initialization
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	int count,z=0,tag=0,root=0,num,*arr;
	double start,end;
	
	FILE* ptr=fopen("data.txt","r");
	fscanf(ptr,"%d",&count);
	if(my_rank==0){
		//Input scanning
		arr=(int*)malloc(count*sizeof(int));
		while(!feof(ptr)){
			fscanf(ptr,"%d",&arr[z++]);
		}
		fclose(ptr);
	}

	int n=count,num_process=size,*batch_size,batches=ceil((float)n/num_process),*displs;
	if (my_rank==0)
	{
		start=MPI_Wtime();
		//This will generalize program for any input size and processes no.
		//The input size need not be multiple of number of processes.
		//Parameters for scatterv.
		batch_size=(int*)malloc(num_process*sizeof(int));
		displs=(int*)malloc(num_process*sizeof(int));
		int count=0;
		for(int i=0;i<num_process;i++){
			batch_size[i]=min((i+1)*batches,n)-min(i*batches,n);
			displs[i]=(count);
			count+=batch_size[i];
		}
	}
	//This will tell how many elements are going to a process and store in "batches" variable
	MPI_Scatter(batch_size,1,MPI_INT,&batches,1,MPI_INT,0,MPI_COMM_WORLD);
	int myarr[batches];
	//This will take batches no of input from proc 0, and store in myarr
	MPI_Scatterv(arr,batch_size,displs,MPI_INT,myarr,batches,MPI_INT,0,MPI_COMM_WORLD);
	
	//This is done so as to avoid copying within same proc 0. 
	if(my_rank==0){
		mergeSort(arr,batches,0,batches-1);
	} else{
		mergeSort(myarr,batches,0,batches-1);
	}
	//After the above call all the processes will have sorted array sub-parts with them

	//This will combine all the sorted array subparts.
	if(my_rank==0){
		for(int i=1;i<size;i++){
			MPI_Recv(arr+displs[i], batch_size[i], MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			merge(arr,0,displs[i]-1,displs[i]+batch_size[i]-1);
		}
		end=MPI_Wtime();
		//This will print the time taken in the process.
		printf("The sorting took %f seconds to run with %d processes\n", end-start,size);
	}else{
		MPI_Send(myarr, batches, MPI_INT, root, tag, MPI_COMM_WORLD);
	}

	//Uncomment this for printing sorted array.
	// if(my_rank==0){
	// 	for(int i=0;i<count;i++){
	// 		printf("%d ",arr[i]);
	// 	}printf("\n");
	// }
	MPI_Finalize();
}
