#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DAYS 5
#define SLOTS 4
#define DIV 3
#define MAX 100

typedef struct{
    int id;
    char name[100];
    char subject[50];
    int maxLectures;
    int taken;
    int dailyCount[DAYS];
    int unavailable[DAYS];
    int maxPerDay;
} Faculty;

typedef struct{
    char subject[50];
    char faculty[100];
    int room;
} Slot;


// RESET FACULTY 
void resetFaculty(Faculty faculty[], int count){
    for(int i=0;i<count;i++){
        faculty[i].taken=0;
        for(int d=0; d<DAYS; d++)
            faculty[i].dailyCount[d]=0;
    }
}


// GENERATE TIMETABLE (FORCED FACULTY CLASH) 
void generateTimetable(Faculty faculty[], int count,
                       Slot timetable[DIV][SLOTS][DAYS],
                       int rooms)
{
    resetFaculty(faculty,count);

    for(int d=0; d<DAYS; d++){
        for(int s=0; s<SLOTS; s++){

            int selectedFaculty=-1;

            for(int div=0; div<DIV; div++){

                int best=-1, maxRemain=-1;

                for(int i=0;i<count;i++){

                    int remain = faculty[i].maxLectures - faculty[i].taken;

                    if(remain > maxRemain &&
                       faculty[i].dailyCount[d] < faculty[i].maxPerDay &&
                       faculty[i].unavailable[d]==0)
                    {
                        maxRemain=remain;
                        best=i;
                    }
                }

                if(best==-1){
                    strcpy(timetable[div][s][d].subject,"FREE");
                    strcpy(timetable[div][s][d].faculty,"NONE");
                    timetable[div][s][d].room=-1;
                    continue;
                }

                if(div==0)
                    selectedFaculty=best;
                else
                    best=selectedFaculty;  // force faculty clash

                faculty[best].taken++;
                faculty[best].dailyCount[d]++;

                timetable[div][s][d].room=(rand()%rooms)+1;

                strcpy(timetable[div][s][d].subject,faculty[best].subject);
                strcpy(timetable[div][s][d].faculty,faculty[best].name);
            }
        }
    }

    printf("\nTimetable Generated (Clash Demo Mode).\n");
}


// DIVISION DISPLAY 
void showDivision(Slot timetable[DIV][SLOTS][DAYS]){
    char divName[DIV]={'A','B','C'};

    for(int div=0; div<DIV; div++){
        printf("\n===== DIVISION %c =====\n",divName[div]);

        for(int d=0; d<DAYS; d++){
            printf("Day %d: ",d+1);

            for(int s=0; s<SLOTS; s++){
                if(timetable[div][s][d].room==-1)
                    printf("[FREE] ");
                else
                    printf("[%s R%d] ",
                           timetable[div][s][d].subject,
                           timetable[div][s][d].room);
            }
            printf("\n");
        }
    }
}


// FACULTY DISPLAY 
void showFaculty(Slot timetable[DIV][SLOTS][DAYS],
                 Faculty faculty[], int count)
{
    for(int f=0; f<count; f++){

        printf("\n===== %s =====\n",faculty[f].name);

        for(int d=0; d<DAYS; d++){
            for(int s=0; s<SLOTS; s++){
                for(int div=0; div<DIV; div++){

                    if(strcmp(timetable[div][s][d].faculty,
                              faculty[f].name)==0)
                    {
                        printf("Day %d Slot %d -> Div %c Room R%d (%s)\n",
                               d+1,s+1,'A'+div,
                               timetable[div][s][d].room,
                               timetable[div][s][d].subject);
                    }
                }
            }
        }
    }
}


// WORKLOAD 
void showWorkload(Faculty faculty[], int count){
    printf("\n===== FACULTY WORKLOAD =====\n");

    for(int i=0;i<count;i++){
        printf("%-20s : %d / %d\n",
               faculty[i].name,
               faculty[i].taken,
               faculty[i].maxLectures);
    }
}


// ROOM DISPLAY 
void showRoomWise(Slot timetable[DIV][SLOTS][DAYS], int rooms){

    for(int r=1; r<=rooms; r++){

        printf("\n===== ROOM R%d =====\n",r);

        for(int d=0; d<DAYS; d++){
            for(int s=0; s<SLOTS; s++){
                for(int div=0; div<DIV; div++){

                    if(timetable[div][s][d].room==r){
                        printf("Day %d Slot %d -> Div %c (%s)\n",
                               d+1,s+1,'A'+div,
                               timetable[div][s][d].subject);
                    }
                }
            }
        }
    }
}


// SET UNAVAILABILITY 
void setUnavailability(Faculty faculty[], int count){

    int id, day;

    printf("Enter Faculty ID: ");
    scanf("%d",&id);

    printf("Enter Day (1-5): ");
    scanf("%d",&day);

    if(day<1 || day>5){
        printf("Invalid day!\n");
        return;
    }

    for(int i=0;i<count;i++){
        if(faculty[i].id==id){
            faculty[i].unavailable[day-1]=1;
            printf("Unavailability updated.\n");
            return;
        }
    }

    printf("Faculty not found!\n");
}


// CLASH DETECTION 
int detectClashes(Slot timetable[DIV][SLOTS][DAYS]){

    int clash=0;

    printf("\n===== DETECTING CLASHES =====\n");
    printf("------------------------------\n");
    printf("|Type       | Day    | Slot    | Divisions    | Details          \n");
    printf("------------------------------\n");

    for(int d=0; d<DAYS; d++){
        for(int s=0; s<SLOTS; s++){
            for(int div1=0; div1<DIV; div1++){
                for(int div2=div1+1; div2<DIV; div2++){

                    Slot t1=timetable[div1][s][d];
                    Slot t2=timetable[div2][s][d];

                    if(t1.room==-1 || t2.room==-1)
                        continue;

                    if(t1.room==t2.room){
                        printf("|   ROOM     |  %d   |  %d   |  %c,%c      |    R%d   \n",
                               d+1,s+1,'A'+div1,'A'+div2,t1.room);
                        clash=1;
                    }

                    if(strcmp(t1.faculty,"NONE")!=0 &&
                       strcmp(t1.faculty,t2.faculty)==0)
                    {
                        printf("|   FACULTY  |  %d   |  %d   |  %c,%c      |      %s   \n",
                               d+1,s+1,'A'+div1,'A'+div2,t1.faculty);
                        clash=1;
                    }
                }
            }
        }
    }

    if(!clash)
        printf("\nNo clashes detected.\n");

    return clash;
}


// CLASH RESOLUTION (FIXED ROOM LOGIC) 
void resolveClashes(Faculty faculty[], int count,
                    Slot timetable[DIV][SLOTS][DAYS],
                    int rooms)
{
    for(int d=0; d<DAYS; d++){
        for(int s=0; s<SLOTS; s++){
            for(int div1=0; div1<DIV; div1++){
                for(int div2=div1+1; div2<DIV; div2++){

                    Slot *t1=&timetable[div1][s][d];
                    Slot *t2=&timetable[div2][s][d];

                    if(t1->room==-1 || t2->room==-1)
                        continue;

                    // ROOM FIX 
                    if(t1->room==t2->room){

                        for(int r=1; r<=rooms; r++){

                            int conflict=0;

                            for(int k=0; k<DIV; k++){

                                if(k==div2) continue;

                                if(timetable[k][s][d].room==r){
                                    conflict=1;
                                    break;
                                }
                            }

                            if(!conflict){
                                t2->room=r;
                                break;
                            }
                        }
                    }

                    // FACULTY FIX 
                    if(strcmp(t1->faculty,"NONE")!=0 &&
                       strcmp(t1->faculty,t2->faculty)==0)
                    {
                        for(int f=0; f<count; f++){

                            int conflict=0;

                            for(int k=0;k<DIV;k++){
                                if(strcmp(timetable[k][s][d].faculty,
                                          faculty[f].name)==0)
                                    conflict=1;
                            }

                            if(!conflict &&
                               faculty[f].unavailable[d]==0)
                            {
                                strcpy(t2->faculty,faculty[f].name);
                                strcpy(t2->subject,faculty[f].subject);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    printf("\nClashes resolved where possible.\n");
}


// MAIN 
int main(){

    Faculty faculty[MAX];
    Slot timetable[DIV][SLOTS][DAYS];

    int facultyCount=0;
    int rooms;

    FILE *file=fopen("faculty_reduced.csv","r");
    if(file==NULL){
        printf("faculty_reduced.csv not found!\n");
        return 0;
    }

    char line[200];
    fgets(line,sizeof(line),file);

    while(fgets(line,sizeof(line),file)){
        Faculty temp={0};

        sscanf(line,"%d,%[^,],%[^,],%d",
               &temp.id,temp.name,temp.subject,&temp.maxLectures);

        temp.maxPerDay=3;

        for(int d=0; d<DAYS; d++){
            temp.dailyCount[d]=0;
            temp.unavailable[d]=0;
        }

        faculty[facultyCount++]=temp;
    }

    fclose(file);

    printf("Loaded %d faculty.\n",facultyCount);

    printf("Enter number of rooms: ");
    scanf("%d",&rooms);

    srand(time(0));

    int choice;

    while(1){

        printf("\n====== MENU ======\n");
        printf("1. Generate Timetable\n");
        printf("2. Set Faculty Unavailability\n");
        printf("3. Division Timetable\n");
        printf("4. Faculty Timetable\n");
        printf("5. Faculty Workload\n");
        printf("6. Room Wise Timetable\n");
        printf("7. Detect Clashes\n");
        printf("8. Resolve Clashes\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        scanf("%d",&choice);

        if(choice==1)
            generateTimetable(faculty,facultyCount,timetable,rooms);
        else if(choice==2)
            setUnavailability(faculty,facultyCount);
        else if(choice==3)
            showDivision(timetable);
        else if(choice==4)
            showFaculty(timetable,faculty,facultyCount);
        else if(choice==5)
            showWorkload(faculty,facultyCount);
        else if(choice==6)
            showRoomWise(timetable,rooms);
        else if(choice==7)
            detectClashes(timetable);
        else if(choice==8)
            resolveClashes(faculty,facultyCount,timetable,rooms);
        else if(choice==0)
            break;
        else
            printf("Invalid choice!\n");
    }

    return 0;
}