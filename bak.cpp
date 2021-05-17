# include <iostream>
# include <cstring>
# include <mysql/mysql.h>
# include <unistd.h>
# include <cdate>


const int backup_interval[2]{30,180};
const char* backup_column[2]{"datet","hdatet"};
const char * folder[2]{"root","home"};

bool mysql_initialize(MYSQL ** connect) {
    *connect = mysql_init (NULL);
    if (*connect == NULL) {
        printf("Error when initializing mysql-server.");
        return 1;
    }
    *connect = mysql_real_connect(*connect,"127.0.0.1","czq01","qaz20000106","czq01",0,NULL,0);
    if (*connect == NULL) {
        printf("Error when connecting to mysql_server.");
        return -1;
    }
    return 0;
}


int backup(MYSQL ** mysql, int choice) {
    own::Date a; a.set_today();
    int datet = (a.year-2000)*365 +a.month*30+a.day; 
    int last_time;

    {
        // inquiry_function
        char * inqury_command = (char *)malloc(50*sizeof(inqury_command));
        sprintf(inqury_command,"SELECT MAX(%s) from backup_time",backup_column[choice]);
        mysql_query(*mysql,inqury_command);
        MYSQL_RES * result = mysql_use_result(*mysql);
        MYSQL_ROW row = mysql_fetch_row(result);
        char * last_t = row[0];
        int last_time = atoi(last_t);
        mysql_free_result(result);
    }

    if (datet - last_time >= backup_interval[choice]) {
        char *backup_command, *sql_command = NULL; 
        backup_command = (char *)malloc(200*sizeof(backup_command));
        sql_command = (char *)malloc(100*sizeof(sql_command));
        if (choice) sprintf(backup_command, "tar cpzf /mnt/h/onedrive/backup/root/backup_%s.tgz --exclude=/proc --exclude=/lost+found --exclude=/mnt --exclude=/home --exclude=/lost+found --exclude=/sys --exclude=/media --exclude=/run /",a.date);
        else sprintf(backup_command,"tar cpzf /mnt/h/onedrive/backup/home/backup_%s.tgz /home",a.date);
        system(backup_command);
        sprintf(sql_command,"INSERT INTO %s values (%s,%d);",backup_column[choice],a.date,datet);
        mysql_query(*mysql, sql_command);
        mysql_commit(*mysql);
        free(backup_command); backup_command = NULL;
        free(sql_command); sql_command = NULL;
    }
    //system("apt update && apt upgrade -y");
    return 0;
}


int if_task_exists() {
    char s; int value;
    FILE *f;
    f = popen("ps -aux | awk 'BEGIN{count=0;} /bak/{count +=1;} END{print count;}'","r");
    sleep(1);
    system("ps -aux | awk '/bak/{print $0}'");
    fgets(&s,2,f);
    fclose(f);
    f = NULL;
    value = s - '0';
    if (value!=3) {return true;}
    return false;
}


int del_backup(int choice) {
    char num; char *date, *command= NULL;
    int num_1;
    FILE *f;
    date = (char *)malloc(15*sizeof(char*));
    command = (char *)malloc(50*sizeof(char*));
    char * inquiry = (char *)malloc(150*sizeof(char*));
    sprintf(inquiry,"stat /mnt/h/Onedrive/backup/%s/backup_* | awk '/^Modify/{print $2}' | awk -F- '{if(NR==1)print $1$2$3} END{print NR}'",folder[choice]);
    f= popen(inquiry,"r");
    fgets(date,9,f);
    fgets(&num,2,f);
    fgets(&num,2,f);
    fclose(f);
    f = NULL;
    num_1 = num -'0';
    if (num_1 > 2) {
        sprintf(command,"rm -rf /mnt/h/Onedrive/backup/%s/backup_%s.tgz",folder[choice],date);
        system(command);
    }
    free(command); command=NULL;
    free(date);date = NULL;
    free(inquiry);inquiry=NULL;
    return 0;
}


int main(int args, char * argv[]) {
    setuid(0);
    /*
    if (if_task_exists()) {
        printf("Task exists.\nAbort.");
        // exit(-1);
    }*/
    int choice = 0;
    if (args) {
        char * a = argv[0];
        if (a[0]=='-' && a[1]=='h') {choice=1;}
        else if (a[0]=='-' && a[1]=='r') {choice=0;}
    }
    const unsigned int sleep_time = 60*60*24*3;
    MYSQL *mysql = NULL;
    bool value;
    value = mysql_initialize(&mysql);
    if (value) {
        printf("Something wrong with mysql_server. Abort.");
        exit(-1);
    }
    backup(&mysql, choice);
    mysql_close(mysql);
    mysql= NULL;
    del_backup(choice);
    //sleep(sleep_time);
    return 0;
}