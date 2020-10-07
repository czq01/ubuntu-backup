# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <mysql/mysql.h>
# include <unistd.h>
# include <stdbool.h>


int mysql_initialize(MYSQL ** connect) {
    *connect = mysql_init (NULL);
    if (*connect == NULL) {
        printf("Error when initializing mysql-server.");
        return -1;
    }
    *connect = mysql_real_connect(*connect,"127.0.0.1","czq01","qaz20000106","czq01",0,NULL,0);
    if (*connect == NULL) {
        printf("Error when connecting to mysql_server.");
        return -1;
    }
    return 0;
}


int get_date(char **s, int *year, int *month, int *day) {
    char * y = NULL; FILE *f;
    y = malloc(10*sizeof(y));
    f = popen("date +\"%y%m%d\"","r");
    strcpy(*s,"20");
    fgets(y,3,f); strcpy(*s,y); *year  = atoi(y);
    fgets(y,3,f); strcat(*s,y); *month = atoi(y);
    fgets(y,3,f); strcat(*s,y); *day   = atoi(y);
    fclose(f); free(y); y = NULL; f = NULL;
    return 0;
}


int backup(MYSQL ** mysql) {
    char * date, *last_t, *backup_command, *sql_command = NULL; 
    char * inqury_command = "SELECT MAX(datet) from backup_time";
    int last_time, month, year, day, datet;
    MYSQL_RES *result; MYSQL_ROW row;
    date = (char *)malloc(16*sizeof(date));
    get_date(&date, &year, &month, &day);
    datet = year*365 + month*30 + day*1;
    mysql_query(*mysql,inqury_command);
    result = mysql_use_result(*mysql);
    row = mysql_fetch_row(result);
    last_t = row[0];
    last_time = atoi(last_t);
    mysql_free_result(result);
    if (datet - last_time >= 30) {
        backup_command = malloc(200*sizeof(backup_command));
        sql_command = malloc(100*sizeof(sql_command));
        sprintf(backup_command, "tar cpzf /mnt/h/backup/backup_20%s.tgz --exclude=/proc --exclude=/lost+found --exclude=/mnt --exclude=/home --exclude=/lost+found --exclude=/sys --exclude=/media --exclude=/run / &",date);
        system(backup_command);
        sprintf(sql_command,"INSERT INTO backup_time values (20%s,%d);",date,datet);
        mysql_query(*mysql, sql_command);
        mysql_commit(*mysql);
        free(backup_command); backup_command = NULL;
        free(sql_command); sql_command = NULL;
    }
    free(date); date = NULL;
    system("apt update&&apt upgrade");
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


int del_backup() {
    char num;
    char *date, *command= NULL;
    int num_1;
    FILE *f;
    date = malloc(15*sizeof(char*));
    command = malloc(50*sizeof(char*));
    f= popen("stat /mnt/h/backup/backup_* | awk '/^Modify/{print $2}' | awk -F- '{if(NR==1)print $1$2$3} END{print NR}'","r");
    fgets(date,9,f);
    fgets(&num,2,f);
    fgets(&num,2,f);
    fclose(f);
    f = NULL;
    num_1 = num -'0';
    if (num_1 >= 2) {
        sprintf(command,"rm -rf /mnt/h/backup/backup_%s.tgz",date);
        system(command);
    }
    free(command); command=NULL;
    free(date);date = NULL;
    return 0;
}


int main() {
    setuid(0);
    if (if_task_exists()) {
        printf("Task exists.\nAbort.");
        exit(-1);
    }
    const unsigned int sleep_time = 60*60*24*3;
    MYSQL *mysql = NULL;
    int value;
    start:
    value = mysql_initialize(&mysql);
    if (value) {
        printf("Something wrong with mysql_server. Abort.");
        exit(-1);
    }
    backup(&mysql);
    mysql_close(mysql);
    mysql= NULL;
    del_backup();
    sleep(sleep_time);
    goto start;
    return 0;
}