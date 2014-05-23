
FMT_CODE *pgmp;


            if (fmt_gener(&pgmp,fmtspecp)) {
                printf("*** fmt_error=%"_LD_"\n",fmt_error);
                fatal(fmtspecp+fmt_errof);
            }

         if(fmt_inter(pgmp,irec,(LONGX)fmtlw,fmtarea,(LONGX)fmtasize) < 0) fatal(...);
            for (p=fmtarea; *p; p++)
                if (*p != '\r') putchar(*p);



         fmt_free(pgmp); pgmp=(FMT_CODE *)NULL;
