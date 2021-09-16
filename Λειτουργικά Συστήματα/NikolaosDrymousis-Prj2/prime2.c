#include "header.h"

#define YES 1
#define NO  0

int prime(int n){
        int i=0, limitup=0;
        limitup = (int)(sqrt((float)n));

        if (n==1) return(NO);
        for (i=2 ; i <= limitup ; i++){
                if ( n % i == 0) return(NO);

        }
        return(YES);
}

int main(int argc, char *argv[]){
        double t1 , t2 , t3 , t4 ;
        struct tms tb1 , tb2, tb3 , tb4 ;
        double ticspersec , ticspersec2 ;

        ticspersec = ( double ) sysconf ( _SC_CLK_TCK ) ;
        t1 = ( double ) times (& tb1 ) ;
        
        int lb=0, ub=0,counter=0;
        int fd = open(argv[1], O_WRONLY, O_NONBLOCK);
        if(fd==-1){
		perror("ERROR\n");
		exit(1);
	}
        
        int *arr = NULL;
        double *time_arr = NULL;

        if ( (argc != 4) ){
                printf("usage: prime1 lb ub\n");
                exit(1); }

        lb=atoi(argv[2]);
        ub=atoi(argv[3]);

        if ( ( lb<1 )  || ( lb > ub ) ) {
                printf("usage: prime1 lb ub %d %d\n",lb,ub);
                exit(1); }

        for (int i=lb ; i <= ub ; i++){
                ticspersec2 = (double) sysconf(_SC_CLK_TCK);
                t3 = (double) times(&tb3);
                if ( prime(i)==YES ){
                        t4 = (double) times(&tb4);
                        counter++;
                        if(counter==1){
			        arr = malloc(sizeof(int)*counter);
                                arr[counter-1] = i;
                                time_arr = malloc(sizeof(double)*counter);
                                time_arr[counter-1] = (t4 - t3) / ticspersec2;
			}else{
				arr = realloc(arr, sizeof(int)*counter);
				arr[counter-1] = i;
                                time_arr = realloc(time_arr, sizeof(double)*counter);
                                time_arr[counter-1] = (t4 - t3) / ticspersec2;
			}
                }
        }

      
        if(write(fd, &counter, sizeof(int)) < 0){
                printf("error\n");
		exit(1);
	}

        if(counter > 0){
                if(write(fd, arr, sizeof(int)*counter) < 0){
                        perror("ERROR :");
                        exit(1);
                }
                if(write(fd, time_arr, sizeof(double)*counter) < 0){
                        perror("ERROR :");
                        exit(1);
                }
        }

        t2 = ( double ) times (& tb2 ) ;

	double p = (t2 - t1 )/ticspersec;

        if(write(fd, &p, sizeof(double)) < 0){
                printf("error\n");
		exit(1);
	}
       
        if(counter > 0){
                free(time_arr);
                free(arr);
        }
        close(fd);
        exit(0);
}
