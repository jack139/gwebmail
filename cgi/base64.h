/* base64.h */

void b64_encode(unsigned char *instr, long len, char *outstr);
long b64_decode(char *instr, long len, unsigned char *outstr);

void qp_decode(char *instr,long len, unsigned char *outstr);
