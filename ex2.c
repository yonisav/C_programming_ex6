 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "org_tree.h"
#define FP_LEN 9


static void print_success(int mask, char *op, char* fingerprint, char* First_Name, char* Second_Name)
{
    printf("Successful Decrypt! The Mask used was mask_%d of type (%s) and The fingerprint was %.*s belonging to %s %s\n",
                       mask, op, FP_LEN, fingerprint, First_Name, Second_Name);
}

static void print_unsuccess()
{
    printf("Unsuccesful decrypt, Looks like he got away\n");
}
//decrypt function letter by letter
int decrypt(int* cipher_bits, Node* node, int mask) {
    for (int j = 0; j < 11; j++) {
        int i = 0;
        while (i < FP_LEN) {
            int xor_encrypted_bit = (int)(node->fingerprint[i] ^ (mask+j));
            if (xor_encrypted_bit != cipher_bits[i]) {
                break;
            }
            i++;
        }
        if (i == FP_LEN) {
            print_success(mask+j, "XOR", node->fingerprint, node->first, node->second);
            return 1;
        }

        i = 0;
        while (i < FP_LEN) {
            int xor_encrypted_bit = (int)(node->fingerprint[i] & (mask+j));
            if (xor_encrypted_bit != cipher_bits[i]) {
                break;
            }
            i++;
        }
        if (i == FP_LEN) {
            print_success(mask+j, "AND", node->fingerprint, node->first, node->second);
            return 1;
        }
    }
	
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s <clean_file.txt> <cipher_bits.txt> <mask_start_s>\n", argv[0]);
        return 0;
    }
    //read the input files
    FILE* clean_file = fopen(argv[1], "r");
    if (clean_file == NULL) {
        printf("Error opening file: %s\n", argv[1]);
        return 0;
    }
	fclose(clean_file);

    FILE* cipher_file = fopen(argv[2], "r");
    if (cipher_file == NULL) {
        printf("Error opening file: %s\n", argv[2]);
        return 0;
    }

    int mask = strtol(argv[3], NULL, 10);
	

    //build the organization
    Org org = build_org_from_clean_file(argv[1]);

    // TODO: attempt to decrypt the file
    int line_number = 0;
    char cipher_bits[10] = { 0 };
    int encrypted_fp[FP_LEN + 1] = { 0 };
    while (fgets(cipher_bits, 10, cipher_file)) {
        int cipher_int = strtol(cipher_bits, NULL, 2);
        encrypted_fp[line_number] = cipher_int;
        line_number++;
    }

	//run on tree and try to decrypt
	int found = 0;
	if (decrypt(encrypted_fp, org.boss, mask)) { return 0; }
	if (decrypt(encrypted_fp, org.left_hand, mask)) { return 0; }
	if (decrypt(encrypted_fp, org.right_hand, mask)) { return 0; }
    Node* current_support;
	//check left hand supports
    if (org.left_hand->supports_head != NULL) {
        current_support = org.left_hand->supports_head;
		if (decrypt(encrypted_fp, current_support, mask)) { return 0; }
        while (current_support->next !=NULL) {
            current_support = current_support->next;
			if (decrypt(encrypted_fp, current_support, mask)) { return 0; }
        }
    }
	//check right hand supports
	if (org.right_hand->supports_head != NULL) {
        current_support = org.right_hand->supports_head;
        if (decrypt(encrypted_fp, current_support, mask)) { return 0; }
        while (current_support->next != NULL) {
            current_support = current_support->next;
            if (decrypt(encrypted_fp, current_support, mask)) { return 0; }
        }
    }
    print_unsuccess();

    // free allocated memory
    fclose(cipher_file);
    free_org(&org);
    
    return 0;
}
