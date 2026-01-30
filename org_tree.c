#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "org_tree.h"


Org build_org_from_clean_file(const char *path) {
    Org org = {0};
	int Node_max = 10;
	int node_count = 0;
	Node* nodes = (Node*)malloc(sizeof(Node) * Node_max); // Allocate memory for 10 nodes
	if (nodes == NULL) {
		printf("Memory allocation failed\n");
		return org;
	}

	//innit org
	org.boss = NULL;
	org.left_hand = NULL;
	org.right_hand = NULL;

	FILE* file = fopen(path, "r");

	if (file == NULL) {
		printf("Error opening file: %s\n", path);
		free(nodes);
		return org;
	}	
	char line[150];
	char enry_lines[4][150];
	int line_number = 0;
	while (fgets(line, sizeof(line), file)) {
		switch (line_number % 5) {
		case 0:
			strcpy(enry_lines[0], line);
			line_number++;
			continue;
		case 1:
			strcpy(enry_lines[1], line);
			line_number++;
			continue;
		case 2:
			strcpy(enry_lines[2], line);
			line_number++;
			continue;
		case 3:
			strcpy(enry_lines[3], line);
			line_number++;
			continue;
		case 4:
			break;
		}
		//handle the boss
		if (strcmp(enry_lines[3], "Position: Boss\n") == 0) {
			sscanf(enry_lines[0], "First Name: %s", nodes[node_count].first);
			sscanf(enry_lines[1], "Second Name: %s", nodes[node_count].second);
			sscanf(enry_lines[2], "Fingerprint: %s", nodes[node_count].fingerprint);
			strcpy(nodes[node_count].position, "Boss");
			org.boss = &nodes[node_count];
		}
		//handle left hand
		else if (strcmp(enry_lines[3], "Position: Left Hand\n") == 0) {
			sscanf(enry_lines[0], "First Name: %s", nodes[node_count].first);
			sscanf(enry_lines[1], "Second Name: %s", nodes[node_count].second);
			sscanf(enry_lines[2], "Fingerprint: %s", nodes[node_count].fingerprint);
			strcpy(nodes[node_count].position, "Left Hand");
			org.left_hand = &nodes[node_count];
			org.boss->left = &nodes[node_count];
			org.left_hand->supports_head = NULL;
		}
		//handle right hand
		else if (strcmp(enry_lines[3], "Position: Right Hand\n") == 0) {
			sscanf(enry_lines[0], "First Name: %s", nodes[node_count].first);
			sscanf(enry_lines[1], "Second Name: %s", nodes[node_count].second);
			sscanf(enry_lines[2], "Fingerprint: %s", nodes[node_count].fingerprint);
			strcpy(nodes[node_count].position, "Right Hand");
			org.right_hand = &nodes[node_count];
			org.boss->right = &nodes[node_count];
			org.right_hand->supports_head = NULL;
		}
		//handle support_left hand
		else if (strcmp(enry_lines[3], "Position: Support_Left\n") == 0) {
			sscanf(enry_lines[0], "First Name: %s", nodes[node_count].first);
			sscanf(enry_lines[1], "Second Name: %s", nodes[node_count].second);
			sscanf(enry_lines[2], "Fingerprint: %s", nodes[node_count].fingerprint);
			strcpy(nodes[node_count].position, "Support_Left");
			nodes[node_count].next = NULL;
			if (org.left_hand->supports_head == NULL) {
				org.left_hand->supports_head = &nodes[node_count];
			}
			else {
				nodes[node_count - 1].next = &nodes[node_count];
			}
		}
		//handle support_right hand
		else if (strcmp(enry_lines[3], "Position: Support_Right\n") == 0) {
			sscanf(enry_lines[0], "First Name: %s", nodes[node_count].first);
			sscanf(enry_lines[1], "Second Name: %s", nodes[node_count].second);
			sscanf(enry_lines[2], "Fingerprint: %s", nodes[node_count].fingerprint);
			strcpy(nodes[node_count].position, "Support_Right");
			nodes[node_count].next = NULL;
			if (org.right_hand->supports_head == NULL) {
				org.right_hand->supports_head = &nodes[node_count];
			}
			else {
				nodes[node_count - 1].next = &nodes[node_count];
			}
		}
		else { 
			printf("Invalid position entered: %s", enry_lines[3]); 
			return org; 
		}


		line_number++;
		node_count++;
		if (node_count >= Node_max) {
			Node_max *= 2;
			nodes = (Node*)realloc(nodes, sizeof(Node) * Node_max);
			if (nodes == NULL) {
				printf("Memory allocation failed\n");
				fclose(file);
				return org;
			}
		}
	}
	return org;
}

void print_tree_order(const Org *org) {
	//print boss details
	fprintf(stdout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: Boss\n\n",
		org->boss->first, org->boss->second, org->boss->fingerprint);
	
	//print left hand details
	fprintf(stdout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: Left Hand\n\n",
		org->left_hand->first, org->left_hand->second, org->left_hand->fingerprint);
	
	//print left hand supports details
	Node* current = org->left_hand->supports_head;
	do {
		fprintf(stdout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: Support_Left\n\n",
			current->first, current->second, current->fingerprint);
		current = current->next;
	} while (current != NULL);
	
	//print right hand details
	fprintf(stdout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: Right Hand\n\n",
		org->right_hand->first, org->right_hand->second, org->right_hand->fingerprint);
	
	//print right hand supports details
	current = org->right_hand->supports_head;
	do {
		fprintf(stdout, "First Name: %s\nSecond Name: %s\nFingerprint: %s\nPosition: Support_Right\n\n",
			current->first, current->second, current->fingerprint);
		current = current->next;
	} while (current != NULL);

}

void free_org(Org *org) {
	free(org->boss);
}

