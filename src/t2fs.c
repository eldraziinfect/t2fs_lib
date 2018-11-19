#include "../include/libs.h"

// Variáveis Globais
static int initiated = 0;
//static int number_of_files = 0;
struct t2fs_superbloco super_bloco;
struct t2fs_record registro;
DESCRITOR_ARQUIVO tabela_de_arquivos[MAX_FILES];

unsigned char *buffer_cluster;
int current_dir_pointer;
//int aux_dir_pointer;
//char *current_dir_name;

//Funções:
void init_data(void);

void init_data(void)
{
    if(initiated!=0)
        return;
    init_tabela_arquivos(tabela_de_arquivos);
    if (!read_sector(0, (unsigned char *) &super_bloco))
    {
        initiated = 1;
        printf("Superbloco\n");
        super_bloco.id[4] = '\0';
        printf("Id: %s\n",super_bloco.id);
        printf("Versao: %X\n",super_bloco.version+0x22);
        printf("Tamanho: %hu - quantidade de setores logicos que formam o superbloco\n",super_bloco.superblockSize);
        printf("Disk Size: %d bytes\n",super_bloco.DiskSize);
        printf("Number of Sectors: %d setores\n",super_bloco.NofSectors);
        printf("Sectors per cluster: %d\n",super_bloco.SectorsPerCluster);
        printf("pFATSectorStart: %d\n",super_bloco.pFATSectorStart);
        printf("RootDirCluster: %d\n",super_bloco.RootDirCluster);
        printf("DataSectorStart: %d\n",super_bloco.DataSectorStart);

        init_fat();
        print_fat();

        buffer_cluster = malloc(super_bloco.SectorsPerCluster*TAM_SETOR);
        init_api_cluster(super_bloco.SectorsPerCluster);
        get_root_dir(buffer_cluster);
        //current_dir = super_bloco.RootDirCluster;
        current_dir_pointer = super_bloco.RootDirCluster;
        printf("chdir .\n");
        chdir2(".");
        //printf("CURRENT DIR: %d\n",current_dir_pointer);

        printf("chdir /dir1\n");
        chdir2("/dir1");
        //printf("CURRENT DIR: %d\n",current_dir_pointer);
        //chdir2("..");
        char *pathname = malloc(TAM_NOME_ARQUIVO);
        printf("getcw = %d\n", getcwd2(pathname,TAM_NOME_ARQUIVO));
        //printf("CURRENT DIR: %d\n",current_dir_pointer);
        free(pathname);

        printf("chdir ..\n");
        chdir2("..");
        //printf("CURRENT DIR: %d\n",current_dir_pointer);

        char *path2 = malloc(TAM_NOME_ARQUIVO);
        getcwd2(path2,TAM_NOME_ARQUIVO);
        printf("getcw = %d\n", getcwd2(path2,TAM_NOME_ARQUIVO));
        free(path2);



        printf("mkdir /dir3\n");
        mkdir2("/dir3");

        printf("mkdir /dir1/dir2\n");
        mkdir2("/dir1/dir2");
        print_sector_as_dir(2);
        print_sector_as_dir(5);
        print_sector_as_dir(11);
        print_sector_as_dir(13);


        printf("cd dir3\n");
        chdir2("/dir3");
        printf("CURRENT DIR: %d\n",current_dir_pointer);



        char *path4 = malloc(TAM_NOME_ARQUIVO);
        printf("getcw = %d\n",getcwd2(path4,TAM_NOME_ARQUIVO));
        printf("Current dir: %s\n",path4);

        printf("cd /dir1/dir2\n");
        chdir2("/dir1/dir2");
        printf("getcw = %d\n",getcwd2(path4,TAM_NOME_ARQUIVO));
        printf("Current dir: %s\n",path4);



        free(path4);
        return;
    }
    else
    {
        printf("%s",ERRO_LEITURA_SUPERBLOCO);
        exit(1);
    }
}


/*-----------------------------------------------------------------------------
Fun��o: Usada para identificar os desenvolvedores do T2FS.
	Essa fun��o copia um string de identifica��o para o ponteiro indicado por "name".
	Essa c�pia n�o pode exceder o tamanho do buffer, informado pelo par�metro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por �\0�.
	O string deve conter o nome e n�mero do cart�o dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identifica��o.
	size -> tamanho do buffer "name" (n�mero m�ximo de bytes a serem copiados).

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size)
{
    char nomes[] = "Gabriel Pakulski da Silva - 00274701\nLuiz Miguel Kruger - 00228271\n";
    int i,flag = 0;
    for(i = 0; i < size; i++)
    {
        if(nomes[i] == '\0')
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0) return -1;
    for(i = 0; i < size && nomes[i] != '\0'; i++)
        name[i] = nomes[i];

    name[i] = '\0';
    return 0;
}


/// TO DO:

/*-----------------------------------------------------------------------------
Fun��o: Criar um novo arquivo.
	O nome desse novo arquivo � aquele informado pelo par�metro "filename".
	O contador de posi��o do arquivo (current pointer) deve ser colocado na posi��o zero.
	Caso j� exista um arquivo ou diret�rio com o mesmo nome, a fun��o dever� retornar um erro de cria��o.
	A fun��o deve retornar o identificador (handle) do arquivo.
	Esse handle ser� usado em chamadas posteriores do sistema de arquivo para fins de manipula��o do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o handle do arquivo (n�mero positivo).
	Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename)
{

    struct	t2fs_record record_vazia;
	char	*temp = "init";
	char	*ultimo_temp = "init";
	char	*temp_dir;
	char	*ultimo_dir;
	int		flag_arquivo_existente = 0;
	int		endereco_FAT;
	unsigned char* vazio = calloc(64, sizeof(unsigned char));
	
    if(!initiated)
	{	init_data();}

	if(filename[0]=='/') // caminho absoluto
	{	temp_dir = super_bloco.RootDirCluster; //muda o diretório para o root
	}
	// faz o percorrimento
	temp = strtok(filename, '/');
	if(temp==NULL) return -1;
	while(temp!=NULL) //ainda tem subdivisoes no nome)
	{	if(flag_arquivo_existente == 1)
		{	return -1; // caminho inválido pois tem '/' depois do nome de um não-dir já existente
		}
		if(temp == ".") //same dir
		{	//não faz nada, já está no diretório certo
		}
		else
		{	if(temp == "..") //father dir
			{	// acha o ponteiro pro diretório pai:
				// percorre o diretório até achar o '..' -- deve ser o segundo arquivo
				temp_dir = seek_dir_in_dir(temp_dir, "..");
			}
			else // sub dir or file name
			{	// percorre o diretório atual, tentando encontrar diretório com o mesmo nome de temp
				ultimo_dir = temp_dir;
				temp_dir = seek_dir_in_dir(temp_dir, temp); /* Talvez isso avacalhe o valor do dir*/
				
				if(seek_file_in_dir(temp_dir, temp)	!= -1)
				{	temp_dir = seek_dir_in_cluster(temp_dir, temp);
				}
				if(seek_file_in_cluster(temp_dir, temp)	!= -1)
					{	flag_arquivo_existente = 1;	// deve-se abrir o arquivo e deixá-lo com 0 bytes
					}
			}
		}
		ultimo_temp = temp;
		temp = strtok(filename, '/');
	}

	if(seek_dir_in_dir(ultimo_dir, ultimo_temp != -1)
	{	return -1; // Erro: endereço aponta para diretório, não para um arquivo
	}
	// valores no fim do loop:
	// ultimo_temp: nome do arquivo
	// ultimo_dir: endereço do diretório do arquivo
	
	//cria o arquivo:
	if(flag_arquivo_existente)
	{	//anula o arquivo na FAT e nos dados
		do
		{	endereco_FAT = get_elemento_fat(ultimo_dir);// acha o registro do arquivo na fat

			for(int j  = 0; j< super_bloco.SectorsPerCluster; j++)
			{	write_sector((cluster_to_sector(endereco_FAT) + j, buffer_setor_vazio); //apaga nos dados
			}
			set_elemento_fat(endereco_FAT, 0x0); // indica que o espaço no disco está livre
		}
		while(endereco_FAT != EOF);
	}
	
	///set_elemento_fat(int cluster, int value);
	// inclui os dados do arquivo na fat - só ocupa um lugar
	endereco_FAT = get_next_livre();
		if(endereco_FAT == -1)
		{	return -1; // não há mais espaço no disco.
		}
	set_elemento_fat(endereco_FAT, EOF);
	
	//inclui os dados no diretório
	record_vazia = vazio;
	
	insert_record(ultimo_dir, record_vazia);
	//Não inclui os dados do arquivo nos dados pq ele inicia com 0 bytes (empty)
	
	
    return 0;//insert_tabela_descritores_de_arquivo(tabela_de_arquivos, record, filename);

}
/*-----------------------------------------------------------------------------
Fun��o:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado � aquele informado pelo par�metro "filename".
Entra:	filename -> nome do arquivo a ser apagado.
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/

int delete2 (char *filename)
{
    if(!initiated)
        init_data();
    return 0;
	
	 
	if(filename[0]=='/') // caminho absoluto
	{	temp_dir = super_bloco.RootDirCluster; //muda o diretório para o root
	}
	// faz o percorrimento
	temp = strtok(filename, '/');
	if(temp==NULL) return -1;
	while(temp!=NULL) //ainda tem subdivisoes no nome)
	{	if(flag_arquivo_existente == 1)
		{	return -1; // caminho inválido pois tem '/' depois do nome de um não-dir já existente 
		}
		if(temp == ".") //same dir
		{	//não faz nada, já está no diretório certo
		}
		else
		{	if(temp == "..") //father dir
			{	// acha o ponteiro pro diretório pai:
				// percorre o diretório até achar o '..' -- deve ser o segundo arquivo
				temp_dir = seek_dir_in_dir(temp_dir, "..");
			}
			else // sub dir or file name
			{	// percorre o diretório atual, tentando encontrar diretório com o mesmo nome de temp
				temp_dir = seek_dir_in_dir(temp_dir, temp); /* Talvez isso avacalhe o valor do dir*/
				
				if(seek_file_in_dir(temp_dir, temp)	!= -1)
					{	flag_arquivo_existente = 1;	// file found!
					}
			}
		}
		ultimo_temp = temp;
		temp = strtok(filename, '/');
	}
	//deleta o arquivo:
	if(flag_arquivo_existente)
	{	//anula o arquivo na FAT e nos dados
		do
		{	endereco_FAT = get_elemento_fat(ultimo_dir);// acha o registro do arquivo na fat

			for(int j  = 0; j< super_bloco.SectorsPerCluster; j++)
			{	write_sector((cluster_to_sector(endereco_FAT) + j, buffer_setor_vazio); //apaga nos dados
			}
			set_elemento_fat(endereco_FAT, 0x0); // indica que o espaço no disco está livre
		}
		while(endereco_FAT != EOF);
	}

	insert_record(ultimo_dir, record_vazia); // elimina do diretório

    return 0;//insert_tabela_descritores_de_arquivo(tabela_de_arquivos, record, filename);

}

/*-----------------------------------------------------------------------------
Fun��o:	Abre um arquivo existente no disco.
	O nome desse novo arquivo � aquele informado pelo par�metro "filename".
	Ao abrir um arquivo, o contador de posi��o do arquivo (current pointer) deve ser colocado na posi��o zero.
	A fun��o deve retornar o identificador (handle) do arquivo.
	Esse handle ser� usado em chamadas posteriores do sistema de arquivo para fins de manipula��o do arquivo criado.
	Todos os arquivos abertos por esta chamada s�o abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, ser� realizada � fornecido pelo valor current_pointer (ver fun��o seek2).
Entra:	filename -> nome do arquivo a ser apagado.
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o handle do arquivo (n�mero positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/

FILE2 open2 (char *filename)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Fecha o arquivo identificado pelo par�metro "handle".
Entra:	handle -> identificador do arquivo a ser fechado
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/

int close2 (FILE2 handle)
{
    if(!initiated)
        init_data();
    return remove_descritor(tabela_de_arquivos,handle);
    /**
    TODO:
        Salvar o conteúdo do arquivo no disco.
    */
}


/*-----------------------------------------------------------------------------
Fun��o:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos s�o colocados na �rea apontada por "buffer".
	Ap�s a leitura, o contador de posi��o (current pointer) deve ser ajustado para o byte seguinte ao �ltimo lido.
Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> n�mero de bytes a serem lidos
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o n�mero de bytes lidos.
	Se o valor retornado for menor do que "size", ent�o o contador de posi��o atingiu o final do arquivo.
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/

int read2 (FILE2 handle, char *buffer, int size)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos est�o na �rea apontada por "buffer".
	Ap�s a escrita, o contador de posi��o (current pointer) deve ser ajustado para o byte seguinte ao �ltimo escrito.
Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> n�mero de bytes a serem escritos
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o n�mero de bytes efetivamente escritos.
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/

int write2 (FILE2 handle, char *buffer, int size)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Fun��o usada para truncar um arquivo.
	Remove do arquivo todos os bytes a partir da posi��o atual do contador de posi��o (CP)
	Todos os bytes a partir da posi��o CP (inclusive) ser�o removidos do arquivo.
	Ap�s a opera��o, o arquivo dever� contar com CP bytes e o ponteiro estar� no final do arquivo
Entra:	handle -> identificador do arquivo a ser truncado
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/

int truncate2 (FILE2 handle)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Reposiciona o contador de posi��es (current pointer) do arquivo identificado por "handle".
	A nova posi��o � determinada pelo par�metro "offset".
	O par�metro "offset" corresponde ao deslocamento, em bytes, contados a partir do in�cio do arquivo.
	Se o valor de "offset" for "-1", o current_pointer dever� ser posicionado no byte seguinte ao final do arquivo,
		Isso � �til para permitir que novos dados sejam adicionados no final de um arquivo j� existente.
Entra:	handle -> identificador do arquivo a ser escrito
	offset -> deslocamento, em bytes, onde posicionar o "current pointer".
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset)
{
    if(!initiated)
        init_data();
	
	//return move_pointer(DESCRITOR_ARQUIVO tabela[], int handle, int offset)
	return move_pointer(DESCRITOR_ARQUIVO tabela[], int handle, int offset)
}


/*-----------------------------------------------------------------------------
Fun��o:	Criar um novo diret�rio.
	O caminho desse novo diret�rio � aquele informado pelo par�metro "pathname".
		O caminho pode ser ser absoluto ou relativo.
	S�o considerados erros de cria��o quaisquer situa��es em que o diret�rio n�o possa ser criado.
		Isso inclui a exist�ncia de um arquivo ou diret�rio com o mesmo "pathname".
Entra:	pathname -> caminho do diret�rio a ser criado
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname)
{
    int next_dir_pointer;
    int aux_dir_pointer;
    struct t2fs_record record;
    struct t2fs_record r2;
    //printf("mkdir %s\n",pathname);
    char* aux = malloc(sizeof(strlen(pathname)));
    char* pch;
    strcpy(aux,pathname);
    aux[0] = '/';
    if(initiated == 0)
        init_data();
    switch(pathname[0])
    {
    case '/': //caminho absoluto.
        aux_dir_pointer = super_bloco.RootDirCluster;
        break;
    case '.': //relativo
        aux_dir_pointer = current_dir_pointer;
        if(pathname[1] == '.')
        {
            aux[1] = '/';
            aux_dir_pointer = seek_dir_in_dir(aux_dir_pointer,"..");
        }
        break;
    default:
        return -1;
    }
    pch = strtok(aux,"/");
    next_dir_pointer = aux_dir_pointer;
    while(pch != NULL)
    {
        //printf("PCH: %s\n",pch);
        next_dir_pointer = seek_dir_in_dir(next_dir_pointer,pch);
        if(next_dir_pointer == -1)
        {
            record.TypeVal = TYPEVAL_DIRETORIO;
            strcpy(record.name,pch);
            record.bytesFileSize = super_bloco.SectorsPerCluster*TAM_SETOR;
            record.clustersFileSize = 1;
            record.firstCluster = get_next_livre();
            set_elemento_fat(record.firstCluster,CLUSTER_EOF);
            //printf("Entao inseri: no dir %d\n", aux_dir_pointer);
            //print_dir(record);
            insert_record(aux_dir_pointer,record);


            record.name[1] = '\0';
            //insert_record(record.firstCluster,record);

            r2.TypeVal = TYPEVAL_DIRETORIO;
            strcpy(r2.name,"..");
            r2.bytesFileSize = super_bloco.SectorsPerCluster*TAM_SETOR;
            r2.clustersFileSize = 1;
            r2.firstCluster = aux_dir_pointer;

            strcpy(record.name,".");
            //printf("Depois foi pra %d e inseri 2 registros %s e %s\n",record.firstCluster,record.name,r2.name);
            //print_dir(record);
            //print_dir(r2);
            insert_record(record.firstCluster,record);
            insert_record(record.firstCluster,r2);
            break;
        }
        pch = strtok(NULL,"/.");
        aux_dir_pointer = next_dir_pointer;
    }
    free(aux);
    return 0;
}

/*-----------------------------------------------------------------------------
Fun��o:	Apagar um subdiret�rio do disco.
	O caminho do diret�rio a ser apagado � aquele informado pelo par�metro "pathname".
	S�o considerados erros quaisquer situa��es que impe�am a opera��o.
		Isso inclui:
			(a) o diret�rio a ser removido n�o est� vazio;
			(b) "pathname" n�o existente;
			(c) algum dos componentes do "pathname" n�o existe (caminho inv�lido);
			(d) o "pathname" indicado n�o � um diret�rio;
Entra:	pathname -> caminho do diret�rio a ser removido
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Altera o diret�rio atual de trabalho (working directory).
		O caminho desse diret�rio � informado no par�metro "pathname".
		S�o considerados erros:
			(a) qualquer situa��o que impe�a a realiza��o da opera��o
			(b) n�o exist�ncia do "pathname" informado.
Entra:	pathname -> caminho do novo diret�rio de trabalho.
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
		Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/

int chdir2 (char *pathname)
{
    //printf("cd %s\n",pathname);
    char* aux = malloc(sizeof(strlen(pathname)));
    char* pch;
    strcpy(aux,pathname);
    pch = strtok(aux,"/.");
    if(initiated == 0)
        init_data();
    switch(pathname[0])
    {
    case '/':
        current_dir_pointer = super_bloco.RootDirCluster;
        break;
    case '.':
        if(pathname[1] == '.')
            current_dir_pointer = seek_dir_in_dir(current_dir_pointer,"..");
        break;
    default:
        return -1;
    }
    while(pch != NULL)
    {
        current_dir_pointer = seek_dir_in_dir(current_dir_pointer,pch);
        if(current_dir_pointer == -1)
        {
            return -1;
        }
        pch = strtok(NULL,"/.");
    }
    free(aux);
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Informa o diret�rio atual de trabalho.
		O "pathname" do diret�rio de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa c�pia n�o pode exceder o tamanho do buffer, informado pelo par�metro "size".
		S�o considerados erros:
			(a) quaisquer situa��es que impe�am a realiza��o da opera��o
			(b) espa�o insuficiente no buffer "pathname", cujo tamanho est� informado por "size".
Entra:	pathname -> buffer para onde copiar o pathname do diret�rio de trabalho
		size -> tamanho do buffer pathname
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
		Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size)
{
    if(!initiated)
        init_data();
    char* buffer = malloc(sizeof(TAM_NOME_ARQUIVO + 1));
    //printf("Ta tentando printar o caminho do %d\n",current_dir_pointer);
    if(get_dir_tree(current_dir_pointer, buffer) == -1)
        return -1;
    //printf("DIRETORIO ATUAL: %s\n",buffer);
    if(strlen(buffer) < size)
    {
        strcpy(pathname,buffer);
        free(buffer);
        return 0;

    }
    else
    {
        free(buffer);
        return -1;
    }
}


/*-----------------------------------------------------------------------------
Fun��o:	Abre um diret�rio existente no disco.
	O caminho desse diret�rio � aquele informado pelo par�metro "pathname".
	Se a opera��o foi realizada com sucesso, a fun��o:
		(a) deve retornar o identificador (handle) do diret�rio
		(b) deve posicionar o ponteiro de entradas (current entry) na primeira posi��o v�lida do diret�rio "pathname".
	O handle retornado ser� usado em chamadas posteriores do sistema de arquivo para fins de manipula��o do diret�rio.
Entra:	pathname -> caminho do diret�rio a ser aberto
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o identificador do diret�rio (handle).
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Realiza a leitura das entradas do diret�rio identificado por "handle".
	A cada chamada da fun��o � lida a entrada seguinte do diret�rio representado pelo identificador "handle".
	Algumas das informa��es dessas entradas ser�o colocadas no par�metro "dentry".
	Ap�s realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a pr�xima entrada v�lida, seguinte � �ltima lida.
	S�o considerados erros:
		(a) qualquer situa��o que impe�a a realiza��o da opera��o
		(b) t�rmino das entradas v�lidas do diret�rio identificado por "handle".
Entra:	handle -> identificador do diret�rio cujas entradas deseja-se ler.
	dentry -> estrutura de dados onde a fun��o coloca as informa��es da entrada lida.
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero ( e "dentry" n�o ser� v�lido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Fecha o diret�rio identificado pelo par�metro "handle".
Entra:	handle -> identificador do diret�rio que se deseja fechar (encerrar a opera��o).
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle)
{
    if(!initiated)
        init_data();
    return 0;
}


/*-----------------------------------------------------------------------------
Fun��o:	Fun��o usada para criar um caminho alternativo (softlink) com o nome dado por linkname (relativo ou absoluto) para um arquivo ou diret�rio fornecido por filename.
Entra:	linkname -> nome do link a ser criado
	filename -> nome do arquivo ou diret�rio apontado pelo link
Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/

int ln2(char *linkname, char *filename)
{
    if(!initiated)
        init_data();
    return 0;
}
