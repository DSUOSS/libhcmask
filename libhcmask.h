#ifndef LIBHCMASK_H
#define	LIBHCMASK_H

#define	ctoi(c)	((c)-'0')


/*
 * Default character sets
 */
extern const char *charset_l;	// loweralpha
extern const char *charset_u;	// upperalpha
extern const char *charset_d;	// digits
extern const char *charset_s;	// special characters
extern const char *charset_a;	// all ascii


typedef struct {
	const char *start;
	const char *current;
} charset_data;

typedef struct {
	unsigned long keyspace_len;
	unsigned long progress;
	charset_data *base_charset;
	charset_data *current_charset;
	charset_data *last_charset;
	char *current_string;
	char *custom_charsets[9];
} mask_t;


/*
 * Initialize the mask structure
 *
 * mask_str:	Base hashcat mask for generation
 * ...:			Additional masks to be used as custom charactersets (1-9)
 *
 * returns: 	Pointer to a mask context
 */
mask_t *init_mask(char *mask_str, ...);


/*
 * Cycles the mask context to the next word
 *
 * m:			Mask context
 *
 * returns:		0 if the end has been reached, 1 if not
 */
inline int next_mask(mask_t *m);


/*
 * Destroys the mask context
 *
 * m:			Mask context
 */
void free_mask(mask_t *m);


#endif