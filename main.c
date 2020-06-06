#include <stdio.h>
#include <termios.h>
#include <string.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KWHT  "\x1B[37m"

enum constantes{ALTURA = 30, ALTURA_MENU =10, LARGURA = 40, LARGURA_MENU=30, INI_X=10, INI_Y=2};

static struct termios old, new;

void initTermios(int echo);
void resetTermios(void);
char getch_(int echo);
char getch(void);
void flush_in();
void clear_scr();
void gotoxy(int x, int y);

void _tela(int altura, int largura);
int _menu();
void _criar_arquivo();
void _abrir_arquivo();
void _editar_arquivo(FILE *arq);
void _exibir_conteudo(FILE *arq);
void _apagar_arquivo();

void main(){
    int opcao;

    for(int i=0;i<1;){
        opcao=_menu();
        if(opcao==1)
            _criar_arquivo();
        else if(opcao==2)
            _abrir_arquivo();
        else if(opcao==3)
            _apagar_arquivo();
        else if(opcao==4){
            i++;
            printf("%s\n", KWHT);
            clear_scr();
        }
    }
}

void initTermios(int echo){
  tcgetattr(0, &old);
  new = old;
  new.c_lflag &= ~ICANON;
  new.c_lflag &= echo ? ECHO : ~ECHO;
  tcsetattr(0, TCSANOW, &new);
}

void resetTermios(void){
  tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo){
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

char getch(void){
  return getch_(0);
}

void flush_in(){
    int ch;
    do {
        ch = fgetc(stdin);
    } while (ch != EOF && ch != '\n');
}

void clear_scr(){
    printf("\033[H\033[J");
}

void gotoxy(int x, int y){
    printf("\033[%d;%dH", (y), (x));
}

void _tela(int altura, int largura){
	char tela[altura][largura];

	for(int i=0;i<altura;i++){
		for(int j=0;j<largura;j++){
			if(i==0 || i==altura-1 || j==0 || j==largura-1){
				tela[i][j]='#';
			}
			else{
				tela[i][j]=' ';
			}
		}
	}

	for(int i=0;i<altura;i++){
		printf("%s\n", KGRN);
		gotoxy(INI_X,INI_Y+i);
		for(int j=0;j<largura;j++){

			printf("%c ", tela[i][j]);
		}
	}

	printf("\n");
	printf("%s", KWHT);

}

int _menu(){

	char opcao;
	int menuY=1;

    for(int i=0;i<1;){
    	clear_scr();
    	_tela(ALTURA_MENU,LARGURA_MENU);
        gotoxy(INI_X+14, INI_Y+2);
        printf("BLOCO DE NOTAS");
        gotoxy(INI_X+6, INI_Y+3);
        printf("%s escolha uma opção (NÚMERO) e pressione ESPAÇO", KWHT);

        gotoxy(INI_X+14, INI_Y+4);
        if(menuY==1)
            printf("%s ", KGRN);
        else
            printf("%s", KWHT);

		printf("1 - CRIAR ARQUIVO");

        gotoxy(INI_X+14, INI_Y+5);
        if(menuY==2)
            printf("%s ", KGRN);
        else
            printf("%s", KWHT);

        printf("2 - ABRIR ARQUIVO");

        gotoxy(INI_X+14, INI_Y+6);
        if(menuY==3)
            printf("%s ", KGRN);
        else
            printf("%s", KWHT);

        printf("3 - APAGAR ARQUIVO");

        gotoxy(INI_X+14, INI_Y+7);
        if(menuY==4)
            printf("%s ", KGRN);
        else
            printf("%s", KWHT);

        printf("4 - SAIR");

	opcao = getch();

        switch(opcao){
            case '1':
                menuY=1;
            break;

            case '2':
                menuY=2;
            break;

            case '3':
                menuY=3;
            break;

            case '4':
                menuY=4;
            break;

            case ' ':
                return menuY;

            break;
            default:
            break;
        }
    }
    return 4;
}

void _criar_arquivo(){

    char nomearq[16];
    char ext_text[5]=".txt";
    char opcao;

    FILE *novoarq;
    FILE *arqlist;

    _tela(ALTURA_MENU,LARGURA_MENU);

    gotoxy(INI_X+2,INI_Y+2);
    printf("Escolha o nome do arquivo (até 10 caracteres)\n");
    gotoxy(INI_X+2,INI_Y+3);
    fgets(nomearq, 11, stdin);
    flush_in();
    nomearq[strcspn(nomearq, "\n" )] = '\0';
    strcat(nomearq,ext_text);

    if(novoarq=fopen(nomearq, "r")){
        gotoxy(INI_X+2,INI_Y+4);
        printf("Já existe um Arquivo com esse Nome!!!\n");
        getch();
    }
    else{
        novoarq=fopen(nomearq, "w");
        fclose(novoarq);

        arqlist=fopen("arqlist.arq", "a");
        fputs(nomearq, arqlist);
        fputs("\n", arqlist);

        fclose(arqlist);

        gotoxy(INI_X+2,INI_Y+4);
        printf("Arquivo criado com sucesso!");

        gotoxy(INI_X+2,INI_Y+5);
        printf("Deseja Editar o Arquivo?(s/n)");
        gotoxy(INI_X+2,INI_Y+6);
        opcao=getch();
        switch(opcao){
            case 's':
            case 'S':
                novoarq=fopen(nomearq, "r+");
                _editar_arquivo(novoarq);
                fclose(novoarq);
        }
    }
    getch();
}

void _abrir_arquivo(){

    char listaarqui[100][16];
    char opcao;
    int pos_arq;

    FILE *arqlist;
    FILE *arq;
    int i=0;

    clear_scr();
    _tela(ALTURA_MENU,LARGURA_MENU);

    if(arqlist=fopen("arqlist.arq", "r+")){

        while(!feof(arqlist)){
            fgets(listaarqui[i],16,arqlist);
            listaarqui[i][strcspn(listaarqui[i], "\n" )] = '\0';
            i++;
        }
        i--;
        fclose(arqlist);
        clear_scr();
        _tela(i+10,LARGURA_MENU);
        gotoxy(INI_X+7,INI_Y+2);
        printf("Arquivos Existentes");
        for(int j=0;j<i;j++){
            gotoxy(INI_X+2,INI_Y+3+j);
            printf("%i %s", j+1, listaarqui[j]);
        }
        getch();

        gotoxy(INI_X+5,INI_Y+3+i);
        printf("Escolha um Arquivo Existente pelo Número");
        gotoxy(INI_X+5,INI_Y+4+i);
        scanf("%i", &pos_arq);
        flush_in();
        pos_arq--;
        if(pos_arq>i){
            gotoxy(INI_X+5,INI_Y+5+i);
            printf("Escolha um Arquivo Existente!!");
        }
        else{
            clear_scr();
            _tela(ALTURA_MENU,LARGURA_MENU);
            gotoxy(INI_X+2,INI_Y+2);
            printf("Deseja Exibir e Editar o Conteúdo do Arquivo?(s/n)");
            opcao=getch();
            switch(opcao){
                case 's':
                case 'S':
                    arq=fopen(listaarqui[pos_arq], "r+");
                    _exibir_conteudo(arq);
                    fclose(arq);
                break;
                default:
                break;
            }
        }
    }
    else{
        clear_scr();
        _tela(ALTURA_MENU,LARGURA_MENU);
        gotoxy(INI_X+2,INI_Y+2);
        printf("Ainda não existem arquivos criados!");
    }
    getch();
}

void _editar_arquivo(FILE *arq){

    char corpo_documento[ALTURA][LARGURA*2];

    char opcao;
    clear_scr();
    _tela(ALTURA, LARGURA);

    gotoxy(INI_X+9,INI_Y);
    printf("Edição Iniciada. Para Encerrar aperte <ENTER> 3 vezes");
    for(int i=0;i<ALTURA-5;i++){
        gotoxy(INI_X+2,INI_Y+2+i);
        fgets(corpo_documento[i], (LARGURA*2), stdin);

        if(corpo_documento[i-2][0]=='\n' && corpo_documento[i-1][0]=='\n' && corpo_documento[i][0]=='\n'){
            gotoxy(INI_X+10,INI_Y+2+i);
            printf("Deseja Encerar?(s/n)");
            opcao=getch();
            switch(opcao){
                case 's':
                case 'S':
                    gotoxy(INI_X+7,(INI_Y+4+i));
                    printf("Edição será Encerrada!!");
                    for(int j=i;j<ALTURA;j++){
                        strcpy(corpo_documento[j], "\n");
                    }
                    i=ALTURA;
                    getch();
                break;
                default:
                    gotoxy(INI_X+10,INI_Y+2+i);
                    printf("                              ");
                break;
            }
        }
    }

    for(int i=0;i<1;){
        clear_scr();
        _tela(ALTURA_MENU, LARGURA_MENU);
        gotoxy(INI_X+10,INI_Y+2);
        printf("Deseja Salvar?(s/n)");
        opcao=getch();
        switch(opcao){
            case 's':
            case 'S':
                for(int i=0; i<ALTURA;i++){
                    fprintf(arq, "%s", corpo_documento[i]);
                }
                gotoxy(INI_X+7,INI_Y+3);
                printf("Dados Salvos com Sucesso!!");
                i++;
                getch();
            break;
            default:
                gotoxy(INI_X+7,INI_Y+3);
                printf("%s", KRED);
                printf("Todos os Dados Serão Perdidos!!");
                printf("%s", KWHT);
                gotoxy(INI_X+7,INI_Y+4);
                printf("Continuar?(s/n)");
                opcao=getch();
                switch(opcao){
                    case 's':
                    case 'S':
                        i++;
                    break;
                    default:
                    break;
                }

            break;
        }
    }
}

void _exibir_conteudo(FILE *arq){

    char corpo_documento[ALTURA][LARGURA*2];

    char opcao;
    int pos_arq;

    clear_scr();
    _tela(ALTURA, LARGURA);

    gotoxy(INI_X+9,INI_Y);
    printf("Exibindo Conteúdo do Arquivo");
    for(int i=0;i<ALTURA-5;i++){
        gotoxy(INI_X+2,INI_Y+2+i);
        fgets(corpo_documento[i], (LARGURA*2), arq);
        printf("%i %s", i+1, corpo_documento[i]);

        if(corpo_documento[i-2][0]=='\n' && corpo_documento[i-1][0]=='\n' && corpo_documento[i][0]=='\n'){
            gotoxy(INI_X+10,INI_Y+2+i);
            printf("Fim do Arquivo");
            gotoxy(INI_X+5,INI_Y+3+i);
            printf("  Deseja Editar parte Arquivo?      ");
            opcao=getch();
            switch(opcao){
                case 's':
                case 'S':
                    for(int k=0;k<1;){
                        gotoxy(INI_X+7,(INI_Y+4+i));
                        printf("Selecione a linha para Edição       ");
                        for(int j=i;j<ALTURA;j++){
                            strcpy(corpo_documento[j], "\n");
                        }
                        gotoxy(INI_X+7,(INI_Y+5+i));
                        scanf("%i", &pos_arq);
                        flush_in();
                        pos_arq--;
                        if(pos_arq>i){
                            gotoxy(INI_X+5,INI_Y+6+i);
                            printf("Escolha uma Linha Válida!!");
                        }
                        else{
                            gotoxy(INI_X+5,INI_Y+6+i);
                            printf("Insira o Novo Conteúdo           ");
                            gotoxy(INI_X+2,INI_Y+7+i);
                            fgets(corpo_documento[pos_arq], (LARGURA*2), stdin);
                            rewind(arq);
                            for(int m=0; m<ALTURA;m++){
                                fprintf(arq, "%s", corpo_documento[m]);
                            }
                        }

                        gotoxy(INI_X+2,INI_Y+8+i);
                        printf("Deseja Editar outra Linha?");
                        opcao=getch();
                        switch(opcao){
                            case 's':
                            case 'S':
                                for(int l=0;l<6;l++){
                                    gotoxy(INI_X+2,(INI_Y+4+l+i));
                                    printf("                                                            ");
                                }
                            break;
                            default:
                                k++;
                                getch();
                            break;
                        }
                    }
                    clear_scr();
                    _tela(ALTURA, LARGURA);
                    gotoxy(INI_X+10,INI_Y+3+i);
                    printf("Deseja Editar todo o Arquivo?");
                    opcao=getch();
                    switch(opcao){
                        case 's':
                        case 'S':
                            rewind(arq);
                            _editar_arquivo(arq);
                        break;
                        default:
                            i=ALTURA;
                            getch();
                        break;
                    }
                break;

                default:
                    clear_scr();
                    _tela(ALTURA, LARGURA);
                    gotoxy(INI_X+10,INI_Y+3+i);
                    printf("Deseja Editar todo o Arquivo?");
                    opcao=getch();
                    switch(opcao){
                        case 's':
                        case 'S':
                            rewind(arq);
                            _editar_arquivo(arq);
                        break;
                        default:
                            i=ALTURA;
                            getch();
                        break;
                    }
                break;
            }
        }
    }
}

void _apagar_arquivo(){
    char listaarqui[100][16];
    char opcao;
    int pos_arq;

    FILE *arqlist;

    int i=0;

    clear_scr();
    _tela(ALTURA_MENU,LARGURA_MENU);

    if(arqlist=fopen("arqlist.arq", "r+")){

        while(!feof(arqlist)){
            fgets(listaarqui[i],16,arqlist);
            listaarqui[i][strcspn(listaarqui[i], "\n" )] = '\0';
            i++;
        }
        i--;
        fclose(arqlist);
        clear_scr();
        _tela(i+10,LARGURA_MENU);
        gotoxy(INI_X+7,INI_Y+2);
        printf("Arquivos Existentes");
        for(int j=0;j<i;j++){
            gotoxy(INI_X+2,INI_Y+3+j);
            printf("%i %s", j+1, listaarqui[j]);
        }
        getch();

        gotoxy(INI_X+5,INI_Y+3+i);
        printf("Escolha um Arquivo Existente pelo Número");
        gotoxy(INI_X+5,INI_Y+4+i);
        scanf("%i", &pos_arq);
        flush_in();
        pos_arq--;
        if(pos_arq>i){
            gotoxy(INI_X+5,INI_Y+5+i);
            printf("Escolha um Arquivo Existente!!");
        }
        else{
            clear_scr();
            _tela(ALTURA_MENU,LARGURA_MENU);
            gotoxy(INI_X+2,INI_Y+2);
            printf("Deseja Apagar o Arquivo e todo seu Conteúdo?(s/n)");
            opcao=getch();
            switch(opcao){
                case 's':
                case 'S':
                    gotoxy(INI_X+7,INI_Y+3);
                    printf("%s", KRED);
                    printf("Todos os Dados Serão Perdidos!!");
                    printf("%s", KWHT);
                    gotoxy(INI_X+7,INI_Y+4);
                    printf("Continuar?(s/n)");
                    opcao=getch();
                    switch(opcao){
                        case 's':
                        case 'S':

                            if (remove(listaarqui[pos_arq])==0){
                                gotoxy(INI_X+7,INI_Y+6);
                                printf("Arquivo Removido Com sucesso");
                                getch();
                                fclose(arqlist);
                                arqlist=fopen("arqlist.arq", "wa");

                                for(;pos_arq<(i-1);pos_arq++){
                                    strcpy(listaarqui[pos_arq], listaarqui[pos_arq+1]);
                                }
                                for(int m=0; m<(i-1);m++){
                                    fputs(listaarqui[m], arqlist);
                                    fputs("\n", arqlist);
                                }
                            }
                            else{
                                gotoxy(INI_X+7,INI_Y+6);
                                printf("Não foi possível apagar o Arquivo.");
                            }
                        break;

                        default:
                        break;
                    }
                break;

                default:
                break;
            }

        }

    }
    else{
        clear_scr();
        _tela(ALTURA_MENU,LARGURA_MENU);
        gotoxy(INI_X+2,INI_Y+2);
        printf("Ainda não existem arquivos criados!");

    }
    fclose(arqlist);
    getch();
}
