#include <u.h>
#include <libc.h>
#include <libsec.h>
#include <auth.h>
#include <authsrv.h>

int consctl = -1;

#define MAX_PASS 35

void
rawon(void)
{
	if(consctl < 0)
		consctl = open("/dev/consctl", OWRITE);
	if(consctl < 0){
		return;
	}
	write(consctl, "rawon", 5);
}

void
rawoff(void)
{
	if(consctl < 0)
		consctl = open("/dev/consctl", OWRITE);
	if(consctl < 0){
		return;
	}
	write(consctl, "rawoff", 6);
}

void
readpassword(char *password)
{
	int r;
	int i = 0;
	char c;

	rawon();
	while (1) {
		r = read(0, &c, 1);
		if (r != 1) return;

		switch (c) {
			case 8:
			case 127:	
				if (i > 0) i--;
				break;
			case 21: // ctrl-u
				i = 0;
				break;
			case '\n':
				password[i] = 0;
				goto out;
			default:
				if (i < MAX_PASS) {
					password[i] = c;
					i++;
				}
		}
	}

out:
	rawoff();
	return;
}

void
main()
{
	char password[MAX_PASS+1] = {0};
	UserPasswd *up;

	up = auth_getuserpasswd(auth_getkey, "proto=pass service=lock user=%q", getuser());
	if (up == nil) {
		print("factotum failure\n");
		return;
	}

	while (strcmp(up->passwd, password) != 0) {
		print("\nenter password: ");
		readpassword(password);
	}
	print("\n");
}
