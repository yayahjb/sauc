       program idx2csv
       implicit none

       character*256 line
       character*20 a, b, c, alpha, beta, gamma
       character*13 sg, z
       character*8 pdbid
       integer as, ae, bs, be, cs, ce
       integer alphas, alphae, betas, betae
       integer gammas, gammae
       integer sgs, sge, zs, ze
       integer pdbids, pdbide
       integer curpos
       integer endpos
       character*128 decomma
       real*8 edgetest, angtest
       integer lineno, olineno

C
C      Write the header for the csv file
C
       write(6,"(3a)"),
     *   "PDB ID,Angle Alpha,Angle Beta,Angle Gamma,",
     *   "Length A,Length B,Length C,",
     *   "Space Group,Z-number"
C
C      skip lines that do not have "CRYST1" in col 9--14
C
       lineno = 0
       olineno = 0
       go to 100
101    write(7, *)"Rejected: ",lineno, olineno,  line
100    lineno = lineno+1
       read (5,'(A128)',ERR=101,END=900) line
       if (line(6:11).ne."CRYST1") go to 100
C
C      Read the PDB ID, a, b, c, alpha, beta, gamma, sg, z
C
       read (line,'(a5)')pdbid

       curpos = 12
       call nexttoken(line,curpos,endpos)
       a = decomma(line(curpos:endpos))
       if (a.eq." ") go to 101
       read(a,*, err=101) edgetest
       if (edgetest < 1.1) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       if (b.eq." ") go to 101
       b = decomma(line(curpos:endpos))
       read(b,*, err=101) edgetest
       if (edgetest < 1.1) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       c = decomma(line(curpos:endpos))
       if (c.eq." ") go to 101
       read(c,*, err=101) edgetest
       if (edgetest < 1.1) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       alpha = decomma(line(curpos:endpos))
       if (alpha.eq." ") go to 101
       read(alpha,*, err=101) angtest
       if (angtest < 5. .or. angtest > 175.) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       beta = decomma(line(curpos:endpos))
       if (beta.eq." ") go to 101
       read(beta,*, err=101) angtest
       if (angtest < 5. .or. angtest > 175.) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       gamma = decomma(line(curpos:endpos))
       if (beta.eq." ") go to 101
       read(gamma,*, err=101) angtest
       if (angtest < 5. .or. angtest > 175.) go to 101
       curpos = endpos+1
       call nexttoken(line,curpos,endpos)
       read(line(curpos:len(line)),'(2a13)') sg,z
       sg = decomma(sg)
       if (sg .eq. " ") go to 101
       if (sg .eq. "1") sg = "P 1"
       z =decomma(z)
       if (z .eq. " ") z = "1"

       call extoken(pdbid, pdbids, pdbide)
       call extoken(a, as, ae)
       call extoken(b, bs, be)
       call extoken(c, cs, ce)
       call extoken(alpha, alphas, alphae)
       call extoken(beta, betas, betae)
       call extoken(gamma, gammas, gammae)
       call extoken(sg, sgs, sge)
       call extoken(z, zs, ze)
C
C      Skip 1,1,1 and 0,0,0 cells (should be none left)
C
       if (a(as:ae).eq."1.000" .and.
     *   b(bs:be).eq."1.000" .and.
     *   c(cs:ce).eq."1.000") go to 101
       if (a(as:ae).eq."0.000" .and.
     *   b(bs:be).eq."0.000" .and.
     *   c(cs:ce).eq."0.000") go to 101

       write(6,"(9a)")
     *   '"'//pdbid(pdbids:pdbide)//'",',
     *   '"'//alpha(alphas:alphae)//'",',
     *   '"'//beta(betas:betae)//'",',
     *   '"'//gamma(gammas:gammae)//'",',
     *   '"'//a(as:ae)//'",',
     *   '"'//b(bs:be)//'",',
     *   '"'//c(cs:ce)//'",',
     *   '"'//sg(sgs:sge)//'",',
     *   '"'//z(zs:ze)//'"'
       olineno = olineno+1
       go to 100
 900   stop
       end

       character*128 function decomma(string)
C
C      remove all internal commas from a token
C
       implicit none
       character*(*) string
       integer i,j

       j = 1
       decomma = " "

       if (string .eq. "n/a") return

       do i = 1,len(string)
         if (string(i:i).ne.",") then
           decomma(j:j)=string(i:i)
           j = j+1
         endif
       enddo
       return
       end

       subroutine nexttoken(string,curpos,endpos)
C
C      Find the next token in string starting at curpos
C
       implicit none

       character*(*) string
       integer curpos, endpos, i, j

       if (len(string).lt.curpos) then
         endpos = curpos
         return
       endif

       do i = curpos,len(string)
         if (string(i:i).ne." ".and.
     *     string(i:i).ne.char(9)) then
           curpos = i
           do j = curpos,len(string)
           if (string(j:j).eq." ".or.
     *       string(j:j).eq.char(9)) then
             endpos = j-1
             return
           endif
           enddo
           endpos = len(string)
           return
         endif
       enddo
       endpos = curpos
       return
       end

       subroutine extoken(string,sstart,send)
C
C      extract the start and end of the token in the string
C
       implicit none

       character*(*) string
       integer sstart
       integer send
       integer i, j

       if (len(string).le.1) then
          sstart = 1
          send = 1
          return
       endif

       do i = 1, len(string)
         if (string(i:i).ne." ".and.
     *      string(i:i).ne.char(9)) then
           sstart = i
           do j = len(string),i,-1
             if (string(j:j).ne." ".and.
     *          string(j:j).ne.char(9)) then
               send = j
               return
             endif
           enddo
         endif
       enddo
       sstart = 1
       send = 1
       return
       end
