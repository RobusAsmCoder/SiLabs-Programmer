USES CRT,DOS,STRINGS;

VAR N,M,I,O,P:LONGINT;
    PATHCO:STRING;
    MAS:ARRAY[0..1024*1024*4-1] OF BYTE;



PROCEDURE SaveData_HEX(FNAM:STRING; VAR _ADR; SIZE:DWORD; MODE:DWORD);
VAR N,M,MM,I,II,O,X,XS,YS,XSS,Y,YY,ISA:LONGINT;
    S:STRING;
    T:TEXT;
BEGIN
        ASSIGN(T,FNAM);
        REWRITE(T);
        I:=SIZE;

        IF MODE=0 THEN BEGIN // C
        END ELSE BEGIN //PASCAL
         Write(T,I-1,'] OF BYTE=(');
        END;
        FOR N:=0 TO I-1 DO BEGIN
//         WriteLn(N);
         X:=MEM[DWORD(@_ADR)+N];
//         WriteLn(X);
         S:=HEX( X );
         DELETE(S,1,6);
//         WRITELN(S);
//         Write(T,MAS[N]);
         IF MODE=0 THEN BEGIN // C
          Write(T,'0x');
         END ELSE BEGIN //PASCAL
          Write(T,'$');
         END;
         Write(T,S);

         IF (I-1)<>N THEN BEGIN
          IF ((N+1) AND 15)=0 THEN BEGIN
           WriteLn(T,',');
          END ELSE BEGIN
           Write(T,',');
          END;
         END;
        END;
        WriteLn(T,'');
        CLOSE(T);

END;

VAR LOSIZ:LONGINT=0;
FUNCTION LoadFile(FNAM:STRING; VAR _ADR):LONGINT;
VAR F:FILE;
BEGIN
        ASSIGN(F,FNAM);
        RESET(F);
        LOSIZ:=FileSize(F);
        BlockRead(F, _ADR, LOSIZ);
        CLOSE(F);
        RESULT:= LOSIZ;
END;



FUNCTION ConvertFile_BIN_TO_HEX(FNAM:STRING; MODE:DWORD):LONGINT;
VAR N,M,I:LONGINT;
BEGIN
        N:=LoadFile(FNAM+'.bin', MAS);
        SaveData_HEX(FNAM+'.inc', MAS, N, MODE);

        TextColor(14);
        Write('Convert File ');
        TextColor(15);
        Write(FNAM+'.bin');
        TextColor(14);
        Write(' TO ');
        TextColor(15);
        Write(FNAM+'.inc');
        TextColor(14);
        Write(' Size: ');
        TextColor(10);
        Write(N);
//        Write('Convert File '+FNAM+'.bin', ' TO ', FNAM+'.inc');

        TextColor(7);
        WriteLn(' ... OK');

        RESULT:=N;

END;


BEGIN
        PATHCO:=ParamSTR(0);
        WHILE PATHCO[LENGTH(PATHCO)]<>'\' DO DELETE(PATHCO,LENGTH(PATHCO),1);
        WriteLn(PATHCO);

        IF ParamSTR(1)<>'' THEN BEGIN
         ConvertFile_BIN_TO_HEX(ParamSTR(1),1);
        END ELSE BEGIN
         ConvertFile_BIN_TO_HEX(PATHCO+'AT_BTRS',1);
        END;


END.





