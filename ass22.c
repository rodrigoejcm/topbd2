#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  FILE *pont_arq; // cria variável ponteiro para o arquivo
  pont_arq = fopen("arquivo_palavra.yap", "w");
  char nomeTabela[40];


  if (con == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "guest", NULL,
          "dvds", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

  if (mysql_query(con, "SHOW TABLES;"))
  {
      finish_with_error(con);
  }

  MYSQL_RES *resultTables = mysql_store_result(con);
  MYSQL_ROW rowTables;
  MYSQL_FIELD *fieldTables;

  while ((rowTables = mysql_fetch_row(resultTables)))
  {


    strcpy(nomeTabela,  rowTables[0]);
    printf("%s ", nomeTabela);
    char consulta[200] = "SELECT * FROM ";
    strcat(consulta,nomeTabela);

    if (mysql_query(con, consulta))
    {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    MYSQL_FIELD *field;

    while ((row = mysql_fetch_row(result)))
    {

        fprintf(pont_arq, "%s(", nomeTabela);
        for(int i = 0; i < num_fields; i++)
        {
            if (i == 0)
            {
               while(field = mysql_fetch_field(result))
               {

               }


            }


            //usando fprintf para armazenar a string no arquivo
            if(i==num_fields-1){
                fprintf(pont_arq, "'%s'", row[i] ? row[i] : "NULL");
            }else{
                fprintf(pont_arq, "'%s',", row[i] ? row[i] : "NULL");
            }

        }
        fprintf(pont_arq, ").\n");
    }
    mysql_free_result(result);


  }

  printf("%s ", "FIM");

  mysql_free_result(resultTables);
  mysql_close(con);
  fclose(pont_arq);
  exit(0);
}
