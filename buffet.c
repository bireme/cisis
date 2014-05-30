                                if (parmindex) {
                                    //
                                    char *ihtba=NULL;
                                    int ihtxen=0
                                    int level=0;
                                    char *ihtp;
                                    char *ihtptr[20];
                                    int   ihtlen[20];
                                    //
                                                                              // a     b    c  d
                                    if (!xbuffet) {
#if CICPP
                                        try { xbuffet = (char *) new char [nbytes]; }
                                        catch (BAD_ALLOC) { xbuffet = (char *)ALLONULL; }
#else /* CICPP */
                                        xbuffet=(char *)ALLOC((ALLOPARM)nbytes);
#endif /* CICPP */
                                        if (xbuffet == (char *)ALLONULL) fatal("fldupdat/procx/Gsplit/marx/ALLOC");
                                    }
                                    *(ihtp=xbuffet)='\0';
                                    if (hasattr) for (p=a+1; p!=b; p++) if (*p == ' ') if (strncmp(p,indattr,indsize)==0) {   //.level="
                                      for (p+=indsize; isdigit(*p); p++ ) level=level*10+((int)(*p)-(int)'0'); break; 
                                    }
                                    if (level>0 && level<20) {
                                      //header
                                      char *a1,*b1=NULL,*c1=NULL;
                                      char *a2,*b2=NULL,*c2=NULL;
                                      *c='\0'; a1=strstr(ba,oarea1);  if (a1) if (a1[oxn1]==' ' || a1[oxn1]=='>') b1=strchr(a1,'>'); *c='<';  // <header ..>
                                      if (b1) { c1=strstr(b1+1,carea1);                                                                       // </header>
                                      if (c1) {
                                        int n;
                                        //copy to headers
                                        *c1='\0'; n=strlen(b1+1); *c1='<';  
                                        if (n>ihtleft) n=ihtleft; if (level>1) *ihtp++=' '; memmove(ihtp,b1+1,n); ihtptr[level-1]=ihtp; ihtp+=n; ihtlen[level-1]=n; *ihtp='\0'; 
                                        ihtptr[level]=NULL; ihtlen[level]=0;
                                        //text
                                        *c='\0'; a2=strstr(ba,oarea2);  if (a2) if (a2[oxn2]==' ' || a2[oxn2]=='>') b2=strchr(a2,'>'); *c='<';  // <text ..>
                                        if (b2) { c2=strstr(b2+2,carea2);                                                                       // </text>
                                        if (c2) {
                                          int n;
                                          //copy to headtxt
                                          *c2='\0'; n=strlen(b2+1); *c2='<'; 
                                          if (n>ihtleft) n=ihtleft; *ihtp++=' '; memmove(ihtp,b2+1,n); ihtptr[level]=ihtp; ihtp+=n; ihtlen[level]=n; *ihtp='\0'; 
                                          //text ptr: ers. ers. ers. txt.I/  ers. ers. ers. txt.II/  ers. ers. ers. txtIII/ 
                                          ihtba=headers[0];
                                          ihtxen=n+1+m;
                                        }//c2
                                      }//c1
                                    }//level


                                    if (max >= 20 && len > max) {  
                                        int m2=max/3*2,m3=max-m2-3;  
                                        char *q=bp+m2,*p=bp+len-m3;
                                        char *z=bp+max-1;          
                                        int left=max;              
                                        int lmin=(xleft)?m2:0;     
                                        while (left-- > lmin)      
                                          if (*z--==' ')           
                                            if (*z=='.' || *z==';')
                                              break;               
                                        if (left >= lmin) {        
                                          len=left+3;              
                                          memmove(bp+left," ..",3); 
                                        } else {                   
                                          len=m2+3+m3;             
                                          memmove(q,"...",3); q+=3; 
                                          while (m3--) *q++= *p++; 
                                        }                          
                                    }                              
