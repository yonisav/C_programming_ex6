#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct entrant {
	char* firstName;
	char* secondName;
	char fingerPrint[10];
	enum position { Boss, Right_Hand, Left_Hand, Support_Right, Support_Left } role;
} entrant;

const char* role_names[] = {"Boss","Right Hand","Left Hand","Support_Right","Support_Left"};

//useful variables
int max_ent;
int n_ent;

//clean up a line by removing "corrupted" characters
char* clean_line(const char* line) {
	size_t len = strlen(line);
	static char cleaned[1025] = {0};
	size_t j = 0;
	for (size_t i = 0; i < len; i++) {
		if (line[i] == '#' || line[i] == '?' || line[i] == '!'
			|| line[i] == '@' || line[i] == '&' || line[i] == '$') {
			continue;
		}
		cleaned[j] = line[i];
		j++;
	}
	if (j != 0) {
		if (cleaned[j - 1] == '\n') {
			cleaned[j - 1] = '\0';
		}
	}
	return cleaned;
}

// Shift the line to the left by indx positions
void clear_line_to_indx(char* line, int indx) {
	int len = strlen(line);
	for (int i = 0; i < len-indx; i++) {
		line[i] = line[indx + i];
	}
	for (int i = len - indx; i < len; i++) {
		line[i] = '\0';
	}
}

int role_to_int(const char* role_str) {
	if (strcmp(role_str, "Boss") == 0) {
		return 0;
	}
	else if (strcmp(role_str, "Right Hand") == 0) {
		return 1;
	}
	else if (strcmp(role_str, "Left Hand") == 0) {
		return 2;
	}
	else if (strcmp(role_str, "Support_Right") == 0) {
		return 3;
	}
	else if (strcmp(role_str, "Support_Left") == 0) {
		return 4;
	}
	else {
		return -1; //invalid role
	}
}
// Check if a fingerprint already exists, return 1 if found, 0 otherwise
int find_fingerprint(entrant* workers, char* fingerprint) {
	for (int i = 0; i < n_ent; i++) {
		if (strcmp(workers[i].fingetPrint, fingerprint) == 0) {
			return 1; 
		}
	}
	return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input_corrupted.txt> <output_clean.txt>\n", argv[0]);
        return 0;
    }
    
	FILE* input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		printf("Error opening file: %s\n", argv[1]);
        return 0;
	}
	FILE* output_file = fopen(argv[2], "w");
	if (output_file == NULL) {
		printf("Error opening file: %s\n", argv[2]);
		fclose(input_file);
		return 0;

	}
	max_ent = 10;
	entrant* workers = (entrant*)(malloc(max_ent * sizeof(entrant)));
	if (workers == NULL) {
		printf("Memory allocation failed\n");
		return 0;
	}

	char buffer[1024];
	char full_line[1024] = { 0 };
	while (fgets(buffer, 1024, input_file)){
		char* cleaned_line = clean_line(buffer); //remove all corrupted characters
				
		/*
		read the cleaned line and extract the entrant details
		do it line by line, until we have the "first name twice" and we can move to the next entrant
		*/
		strncat(full_line, cleaned_line, strlen(cleaned_line)); 
		char* FN_loc = strstr(full_line, "First Name: "); //first name location
		if (FN_loc != NULL) {
			char* s_FN_loc = strstr(FN_loc +1, "First Name:");
			if (s_FN_loc != NULL) {

				char* SN_loc = strstr(FN_loc, "Second Name: "); //second name location
				char* FP_loc = strstr(SN_loc, "Fingerprint: "); //fingerprint location
				char* PO_loc = strstr(FP_loc, "Position: "); //position location
				
				//check for duplicate fingerprint
				char fingerprint[10] = { 0 };
				int ex_len = strlen("Fingerprint: ");
				int len = PO_loc - FP_loc - ex_len;
				strncpy(fingerprint, FP_loc + ex_len, len);
				if (find_fingerprint(workers, fingerprint)) {
					clear_line_to_indx(full_line, s_FN_loc - full_line);
					continue;
				}
				
				//first name
				ex_len = strlen("First Name: ");
				len = SN_loc - FN_loc - ex_len;
				workers[n_ent].firstName = (char*)malloc((len + 1));
				if (workers[n_ent].firstName == NULL) {
					printf("Memory allocation failed\n");
					return 0;
				}
				strncpy(workers[n_ent].firstName, FN_loc + ex_len, len);
				workers[n_ent].firstName[len] = '\0';
				
				//second name
				ex_len = strlen("Second Name: ");
				len = FP_loc - SN_loc - ex_len;
				workers[n_ent].secondName = (char*)malloc(len + 1);
				if (workers[n_ent].secondName == NULL) {
					printf("Memory allocation failed\n");
					return 0;
				}
				strncpy(workers[n_ent].secondName, SN_loc + ex_len, len);
				workers[n_ent].secondName[len] = '\0';
				
				//fingerprint
				strcpy(workers[n_ent].fingetPrint, fingerprint);
				
				//position
				ex_len = strlen("Position: ");
				len = s_FN_loc - PO_loc - ex_len;
				char role_str[15] = { 0 };
				strncpy(role_str, PO_loc + ex_len, len);
				workers[n_ent].role = role_to_int(role_str);
				
				//increment entrant count, reallocate if needed
				n_ent++;
				if (n_ent >= max_ent) {
					max_ent *= 2;
					workers = (entrant*)realloc(workers, max_ent * sizeof(entrant));
					if (workers == NULL) {
						printf("Memory allocation failed\n");
						return 0;
					}
				}
				clear_line_to_indx(full_line, s_FN_loc - full_line);
			}
		}
	}
	//process the last entrant if exists
	char* FN_loc = strstr(full_line, "First Name: "); //first name location
	if (FN_loc != NULL) {
		char* s_FN_loc = FN_loc + strlen(FN_loc); //simulate "next" first name location
		if (s_FN_loc != NULL) {

			char* SN_loc = strstr(FN_loc, "Second Name: "); //second name location
			char* FP_loc = strstr(SN_loc, "Fingerprint: "); //fingerprint location
			char* PO_loc = strstr(FP_loc, "Position: "); //position location

			//check for duplicate fingerprint
			char fingerprint[10] = { 0 };
			int ex_len = strlen("Fingerprint: ");
			int len = PO_loc - FP_loc - ex_len;
			strncpy(fingerprint, FP_loc + ex_len, len);
			if (!find_fingerprint(workers, fingerprint)) {
				//first name
				ex_len = strlen("First Name: ");
				len = SN_loc - FN_loc - ex_len;
				workers[n_ent].firstName = (char*)malloc(len + 1);
				if (workers[n_ent].firstName == NULL) {
					printf("Memory allocation failed\n");
					return 0;
				}
				strncpy(workers[n_ent].firstName, FN_loc + ex_len, len);
				workers[n_ent].firstName[len] = '\0';

				//second name
				ex_len = strlen("Second Name: ");
				len = FP_loc - SN_loc - ex_len;
				workers[n_ent].secondName = (char*)malloc(len + 1);
				if (workers[n_ent].secondName == NULL) {
					printf("Memory allocation failed\n");
					return 0;
				}
				strncpy(workers[n_ent].secondName, SN_loc + ex_len, len);
				workers[n_ent].secondName[len] = '\0';

				//fingerprint
				strcpy(workers[n_ent].fingetPrint, fingerprint);

				//position
				ex_len = strlen("Position: ");
				len = s_FN_loc - PO_loc - ex_len;
				char role_str[15] = { 0 };
				strncpy(role_str, PO_loc + ex_len, len);
				workers[n_ent].role = role_to_int(role_str);

				//increment entrant count, reallocate if needed
				n_ent++;
			}
		}
	}

	//print worker details by roles order
	for(int i = 0; i < 5; i++) {
		for (int j = 0; j < n_ent; j++) {
			if (workers[j].role == i) {
				fprintf(output_file, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: %s\n\n",
					workers[j].firstName,
					workers[j].secondName,
					workers[j].fingetPrint,
					role_names[workers[j].role]);
			}
		}

	}

	fclose(input_file);
	fclose(output_file);

	for (int i = 0; i < n_ent; i++) {
		free(workers[i].firstName);
		free(workers[i].secondName);
	}
	free(workers);


    return 0;
}
