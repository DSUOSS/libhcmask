#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "libhcmask.h"


const char *charset_l = "abcdefghijklmnopqrstuvwxyz";
const char *charset_u = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *charset_d = "0123456789";
const char *charset_s = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
const char *charset_a = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";


/*
 * Take a hashcat mask and create a full characterset string
 *
 * mask:		string containing a hashcat mask
 *
 * returns:		string containing each character in the custom characterset
 */
char *custom_charset(char *mask) {

	char *tmp;

	if(!mask)
		return NULL;

	tmp = calloc(strlen(charset_a) * strlen(mask), sizeof(char));

	while(*mask != '\0') {
		if(*mask == '?') {
			mask++;
			switch(*mask) {
				case 'l': strcat(tmp, charset_l); break;
				case 'u': strcat(tmp, charset_u); break;
				case 'd': strcat(tmp, charset_d); break;
				case 's': strcat(tmp, charset_s); break;
				case 'a': strcat(tmp, charset_a); break;
				case '?': strncat(tmp, mask-1, sizeof(char)); break;
				default: return NULL;
			}
		} else {
			strncat(tmp, mask, sizeof(char));
		}
		mask++;
	}

	return tmp;

}


/*
 * Gathers up all the current characters and appends them to the current string
 *
 * m:		Mask structure
 */
inline void render_mask(mask_t* m) {

	char* current_char = m->current_string;
	charset_data* current_charset = m->base_charset;

	while(current_charset->current != NULL) {
		*current_char = *(current_charset->current);
		current_char++;
		current_charset++;
	}

}


/*
 * Initialize the mask structure
 *
 * mask_str:	Base hashcat mask for generation
 * ...:			Additional masks to be used as custom charactersets (1-9)
 *
 * returns:		Pointer to a mask context
 */
mask_t *init_mask(char* mask_str, ...) {

	mask_t *m;
	charset_data *current_charset;
	char c[2] = {0};
	char *ptr;
	int num_custom_charsets, i;
	va_list ap;

	m = calloc(1, sizeof(mask_t));
	if(!m)
		return NULL;

	m->keyspace_len = 1;
	m->progress = 1;
	m->current_string = calloc(1, strlen(mask_str) + 1);
	m->base_charset = calloc((strlen(mask_str) + 1), sizeof(charset_data));

	if(!m->base_charset)
		return NULL;

	num_custom_charsets = 0;
	ptr = mask_str;
	while(*ptr != '\0') {
		if(*ptr == '?' && *(ptr+1) >= '1' && *(ptr+1) <= '9') {
			ptr++;
			if(ctoi(*ptr) > num_custom_charsets)
				num_custom_charsets = ctoi(*ptr);
		}
		ptr++;
	}

	va_start(ap, mask_str);
	for(i=0; i<num_custom_charsets; i++) {
		m->custom_charsets[i] = custom_charset(va_arg(ap, char *));
	}
	va_end(ap);

	current_charset = m->base_charset;

	while(*mask_str != '\0') {
		if(*mask_str == '?') {
			mask_str++;
			switch(*mask_str) {
				case 'l':
					current_charset->start = charset_l;
					current_charset->current = charset_l;
					m->keyspace_len *= 26;
					break;
				case 'u':
					current_charset->start = charset_u;
					current_charset->current = charset_u;
					m->keyspace_len *= 26;
					break;
				case 'd':
					current_charset->start = charset_d;
					current_charset->current = charset_d;
					m->keyspace_len *= 10;
					break;
				case 's':
					current_charset->start = charset_s;
					current_charset->current = charset_s;
					m->keyspace_len *= 32;
					break;
				case 'a':
					current_charset->start = charset_a;
					current_charset->current = charset_a;
					m->keyspace_len *= 94;
					break;
				case '?':
					current_charset->start = NULL;
					c[0] = *mask_str;
					current_charset->current = strstr(charset_a, c);
					break;
				default:
					if(*mask_str >= '1' && *mask_str <= '9') {
						if(m->custom_charsets[ctoi(*mask_str)-1] == NULL) {
							return NULL;
						} else {
							current_charset->start = m->custom_charsets[ctoi(*mask_str)-1];
							current_charset->current = m->custom_charsets[ctoi(*mask_str)-1];
							m->keyspace_len *= strlen(m->custom_charsets[ctoi(*mask_str)-1]);
						}
					} else {
						return NULL;
					}
			}
		} else {
			current_charset->start = NULL;
			c[0] = *mask_str;
			current_charset->current = strstr(charset_a, c);
		}
		current_charset++;
		mask_str++;
	}

	m->current_charset = current_charset;
	m->last_charset = current_charset;
	render_mask(m);

	return m;

}


/*
 * Cycles the mask context to the next word
 *
 * m:			Mask context
 *
 * returns:		0 if the end has been reached, 1 if not
 */

inline int next_mask(mask_t *m) {

	if (m->keyspace_len == m->progress) {
		return 0;
	}

	for(;;) {
		if(m->current_charset->start == NULL) {
			m->current_charset--;
			continue;
		}
		m->current_charset->current++;
		if(*(m->current_charset->current) == '\0') {
			m->current_charset->current = m->current_charset->start;
			m->current_charset--;
		} else {
			break;
		}
	}

	render_mask(m);
	m->current_charset = m->last_charset;
	m->progress++;

	return 1;

}


/*
 * Destroys the mask context
 *
 * m:		Mask context
 */
void free_mask(mask_t *m) {

	int c;

	free(m->current_string);
	free(m->base_charset);
	for(c=0; c<9; c++) {
		if(m->custom_charsets[c] != NULL)
			free(m->custom_charsets[c]);
	}
	free(m);

}

