/* To compile with MSVC:
 * cl main.c /nologo /Zi /link gdi32.lib user32.lib ws2_32.lib
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdbool.h>

unsigned long djb2_hash(const char* str)
{
     unsigned long hash = 5381;
     int c;
	  
     while (c = *str++) {
          hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
     }
     return hash;
}

void convert_html_to_ansi(char *str)
{
     int len = strlen(str) + 1;
     while (*str != '\0') {
          if (*str == '&') {
               if(str[1] == 'a' &&
                  str[2] == 'm' &&
                  str[3] == 'p' &&
                  str[4] == ';')
               {
                    memmove(str+1, str+5, len);
                    len -= 5;
                    ++str;
               } else if(str[1] == 'a' &&
                         str[2] == 'p' &&
                         str[3] == 'o' &&
                         str[4] == 's' &&
                         str[5] == ';')
               {
                    *str = '\'';
                    memmove(str+1, str+6, len);
                    len -= 6;
                    ++str;
               }
          } else {
               ++str;
               --len;
          }
     }
}

int get_server_response_for_msg(char *out, int out_sz, char *reqmsg, int reqmsg_len)
{
     char stream_ip[16] = "151.80.97.38";
     unsigned short stream_port = 8224;

     SOCKET sock;
     struct sockaddr_in addr;
     addr.sin_family = AF_INET;
     addr.sin_port = stream_port;
     addr.sin_addr.S_un.S_addr = inet_addr(stream_ip);

     sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     
     int bytes_transf;
     bytes_transf = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
     if (bytes_transf != 0) {
          printf("Couldn't connect.. err: %i\n", WSAGetLastError());
          return -1;
     }

     bytes_transf = send(sock, reqmsg, reqmsg_len, 0);
     if (bytes_transf == SOCKET_ERROR) {
          printf("Couldn't send.. err: %i\n", WSAGetLastError());
          return -1;
     }

     bytes_transf = recv(sock, out, out_sz, 0);
     if (bytes_transf == SOCKET_ERROR) {
          printf("Couldn't recieve.. err: %i\n", WSAGetLastError());
          return -1;
     }
     closesocket(sock);

     return bytes_transf;
}

void append_string_to_file(char *fname, char *str)
{
     FILE *file = fopen(fname, "a");
     fprintf(file, "%s\n", str);
     fclose(file);     
}

void extract_songtitle(char *out, char *in, int in_size)
{
     /* NOTE: This entire function aassumes a format that was the default at the time of creation.
	  * If this format changes, this function will probably brake. (format of 7.hml file)
	 */
     in += in_size;
     
     while (*--in != '<') {;} /* skip </html> */
     while (*--in != '<') {;} /* skip </body> */
     *in = '\0';
     while ( 1 ) {
          while (*in != ',') {--in;} /* find start of name */
          if (!isdigit(*(in-1)) ) { /* if before the comma there is not a number then it's not the end of the song_title */
               --in;
               continue;
          } else {
               break;
          }
     }
     strcpy(out, in+1);
}

int main ()
{     
     struct WSAData wsadata;
     WSAStartup(MAKEWORD(2,2), &wsadata);

     char reqmsg[] = "GET /7.html HTTP/1.1 \r\n\r\n";
     int reqmsg_len = strlen(reqmsg);
     
     char song_title[128];
     unsigned long song_title_hash = 0;
     unsigned long prev_song_title_hash = 0;

     char buffer[512];    
     int bytes_transf;
     int times_missed = 0;      /* Used in console to track the efficiency of the chosen sleep time */

     strcpy(song_title, "Tim Schaufert amp; CASHFORGOLD - Tim Schaufert &amp; CASHFORGOLD- I_ll run away");
     convert_html_to_ansi(song_title);
     
     while ( 1 ) {
          if ((bytes_transf = get_server_response_for_msg(buffer, 512, reqmsg, reqmsg_len)) == -1) {
               break;           /* ERROR */
          }
          extract_songtitle(song_title, buffer, bytes_transf);
          convert_html_to_ansi(song_title);
          song_title_hash = djb2_hash(song_title);

          if (prev_song_title_hash != song_title_hash) {
               append_string_to_file("nebula_songs.txt", song_title);
               printf("(%i)|%s \n", times_missed, song_title);
               fflush(stdout);

               prev_song_title_hash = song_title_hash;
               times_missed = 0;
          } else {
               ++times_missed;
          }
          
#ifdef DEBUG
          Sleep(20000);
#endif
          Sleep(120000);        /* 2 mins */
     }
     
     WSACleanup();
     puts("Closing... (Press a key to exit)");
     getchar();
     return 0;
}


/////// Used to start streaming data ///////
//     strcpy(buffer, "GET /stream HTTP/1.1 \r\n Host: 151.80.97.38:8224 \r\n UserAgent: Ultravox/2.1 \r\n Accept: */* \r\n Icy-MetaData: 1 \r\n\r\n");
