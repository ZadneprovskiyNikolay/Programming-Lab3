#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct IntArray {
    int* arr;
    size_t size;
    size_t used;
};

void init_IntArray(struct IntArray* a, size_t initial_size) {
    a->arr = (int*) malloc(initial_size * sizeof(int));    
    a->size = initial_size;
    a->used = 0;
}

void append_IntArray(struct IntArray* a, int element) {
    if (a->used == a->size) {
        a->size *= 2;
        a->arr = (int*) realloc(a->arr, a->size * sizeof(int));
    }
    a->arr[a->used++] = element;
}

void free_IntArray(struct IntArray* arr) {
    free(arr->arr);
    arr->arr = NULL;
    arr->used = arr->size = 0;
}

int month_to_num(char* month) {    
    if (!strcmp(month, "Jan")) return 0;
    else if (!strcmp(month, "Feb")) return 1;
    else if (!strcmp(month, "Mar")) return 2;
    else if (!strcmp(month, "Apr")) return 3;
    else if (!strcmp(month, "May")) return 4;
    else if (!strcmp(month, "Jun")) return 5;
    else if (!strcmp(month, "Jul")) return 6;
    else if (!strcmp(month, "Aug")) return 7;
    else if (!strcmp(month, "Sep")) return 8;
    else if (!strcmp(month, "Oct")) return 9;
    else if (!strcmp(month, "Nov")) return 10;
    else if (!strcmp(month, "Dec")) return 11;
    return -1;
}

void num_to_month(int month_num, char* str) {
    const char* res;
    if (month_num == 0) res = "Jan";
    else if (month_num == 1) res = "Feb";
    else if (month_num == 2) res = "Mar";
    else if (month_num == 3) res = "Apr";
    else if (month_num == 4) res = "May";
    else if (month_num == 5) res = "Jun";
    else if (month_num == 6) res = "Jul";
    else if (month_num == 7) res = "Aug";
    else if (month_num == 8) res = "Sep";
    else if (month_num == 9) res = "Oct";
    else if (month_num == 10) res = "Nov";
    else if (month_num == 11) res = "Dec";
    strcpy(str, res);
}

typedef struct {
    char** array;
    size_t used;
    size_t size;
} StringArray;

void init_StringArray(StringArray *a, size_t initial_size) {
    a->array = (char**) malloc(initial_size * sizeof(char*));
    a->used = 0;
    a->size = initial_size;
}

void cpyinsert_StringArray(StringArray *a, char* element) {
    if (a->used == a->size) {
    a->size *= 2;
    a->array = (char**) realloc(a->array, a->size * sizeof(int));
    }
    char* str_cpy = (char*) malloc(sizeof(char) * strlen(element) + 1);
    strcpy(str_cpy, element);
    a->array[a->used++] = str_cpy;
}

void get_time_str(time_t time_sec, char* str) {
    struct tm* t = localtime(&time_sec);    
    char month[4], mday[3], hour[3], minute[3], second[3];    
    num_to_month(t->tm_mon, month);
    if (t->tm_mday < 10)
        sprintf(mday, "0%d", t->tm_mday);
    else
        sprintf(mday, "%d", t->tm_mday);       
    if (t->tm_hour < 10)
        sprintf(hour, "0%d", t->tm_hour);
    else
        sprintf(hour, "%d", t->tm_hour);
    if (t->tm_min < 10)
        sprintf(minute, "0%d", t->tm_min);
    else
        sprintf(minute, "%d", t->tm_min);    
    if (t->tm_sec < 10)
        sprintf(second, "0%d", t->tm_sec);
    else
        sprintf(second, "%d", t->tm_sec);

    sprintf(str, "%s/%s/%d:%s:%s:%s", mday, month, t->tm_year + 1900, hour, minute, second);
}

char* biggest_req_freq_interval(char* res, int interval, struct IntArray req_time_arr) {    
    int total_req = req_time_arr.used;
    int max_req = 0;
    int first, last; // First and last requset index of the period of given length with maximum requests
    // Find number of requsts in first interval of length interval_sec      
    time_t max_time = req_time_arr.arr[0] + interval;
    for (; max_req <= total_req && req_time_arr.arr[max_req] <= max_time; max_req++);
    first = 0;
    last = max_req - 1;    

    for (int line_idx = max_req; line_idx < total_req; line_idx++) {
        time_t period_start = req_time_arr.arr[line_idx] - interval;
        if (req_time_arr.arr[line_idx - max_req] >= period_start) {
            // There is more than max_req requests in period 
            // [ req_time_arr.arr[line_idx] - interval; req_time_arr.arr[line_idx] ].
            // Update max_req.            
            int i;
            for (i = line_idx - max_req; i > 0 && req_time_arr.arr[i] >= period_start; i--)
                max_req++;
            first = i + 1;
            last = line_idx;
        }
    }

    char first_time_str[21], last_time_str[21];
    get_time_str(req_time_arr.arr[first], first_time_str);
    get_time_str(req_time_arr.arr[last], last_time_str);
    sprintf(res, "%s - %s", first_time_str, last_time_str);
    printf("max request frequency period: %s (requests in period: %d)\n", res, max_req);
}

void free_StringArray(StringArray *a) {
    for (size_t i = 0; i < a->size; a++) {
        free(a->array[i]);
    }
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

int main(void) {
    struct IntArray req_time_arr;
    init_IntArray(&req_time_arr, 2000000);

    FILE* input_file = fopen("access_log_Jul95", "r");
    char* line = NULL;

    if (input_file == NULL)
        exit(EXIT_FAILURE);

    size_t err_count = 0;
    StringArray err_requests;
    init_StringArray(&err_requests, 10);
    
    size_t len = 0;
    int read;
    printf("Loading input...\n");
    for (; (read = getline(&line, &len, input_file)) != -1;) {
                
        int i = 0;
        for (; i < read && line[i] != '['; i++);
        i++;    // i is on the begining of date time part                
        int day, month, year, hour, minute, second;
        day = strtol(line + i, NULL, 10);
        year = strtol(line + i + 7, NULL, 10) - 1900;        
        hour = strtol(line + i + 12, NULL, 10);
        minute = strtol(line + i + 15, NULL, 10);
        second = strtol(line + i + 18, NULL, 10);
        char month_str[4];
        month_str[0] = line[i + 3];
        month_str[1] = line[i + 4];
        month_str[2] = line[i + 5];
        month_str[3] = '\0';
        month = month_to_num(month_str);
        
        // Convert date and time to seconds
        struct tm time;
        time.tm_mday = day;
        time.tm_mon = month;
        time.tm_year = year;
        time.tm_hour = hour;
        time.tm_min = minute;
        time.tm_sec = second;
        time_t time_sec = mktime(&time);
        if (time_sec != -1)
            append_IntArray(&req_time_arr, time_sec);

        // Extract response satus code of the request and increment err_count if error is 5**
        int resp_code;
        for (; i < read && line[i] != '\"'; i++);
        i++;
        for (; i < read && line[i] != '\"'; i++);
        i += 2; // i is on the begining of status code
        char resp_code_buf[4];
        resp_code_buf[3] = '\0';
        for (int x = 0; i + x < read && x < 3; x++) {
            resp_code_buf[x] = line[i + x];            
        }        
        resp_code = strtol(resp_code_buf, NULL, 10);        
        if (resp_code / 100 == 5) {
            err_count++;        
            cpyinsert_StringArray(&err_requests, line);
        }
        
    }
    printf("Loading complete. Read %d valid requests.\n", req_time_arr.used);
    printf("The number of 5** errors: %d\n", err_count);
    printf("Errored requests:\n");
    for (size_t i = 0; i < err_requests.used; i++) {
        printf("%s\n", err_requests.array[i]);
    }

    int period_sec;
    char res[44];    
    while (1) {
        printf("Enter period in seconds: ");
        scanf("%d", &period_sec);        
        biggest_req_freq_interval(res, period_sec, req_time_arr); 
    }   


    free_IntArray(&req_time_arr);
    free_StringArray(&err_requests);    
    fclose(input_file);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}