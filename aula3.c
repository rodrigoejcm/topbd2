#include "Yap/YapInterface.h"
#include "unistd.h"
#include "sys/types.h"

static int start_n100(void);
static int continue_n100(void);

typedef struct {
    YAP_Term next_solution;  /* the next solution */
   } n100_data_type;

n100_data_type *n100_data;


static int start_n100(void)
{
      YAP_Term t = YAP_ARG1;
      YAP_PRESERVE_DATA(n100_data,n100_data_type);
      if(YAP_IsVarTerm(t)) {
          n100_data->next_solution = YAP_MkIntTerm(0);
          return continue_n100();
       }
      if(!YAP_IsIntTerm(t) || YAP_IntOfTerm(t)<0 || YAP_IntOfTerm(t)>100) {
          YAP_cut_fail();
      } else {
          YAP_cut_succeed();
      }
}

static int continue_n100(void)
{
      int n;
      YAP_Term t;
      YAP_Term sol = YAP_ARG1;
      YAP_PRESERVED_DATA(n100_data,n100_data_type);
      n = YAP_IntOfTerm(n100_data->next_solution);
      if( n == 100) {
           t = YAP_MkIntTerm(n);
           YAP_Unify(sol,t);
           YAP_cut_succeed();
        }
       else {
           YAP_Unify(sol,n100_data->next_solution);
           n100_data->next_solution = YAP_MkIntTerm(n+1);
           return(TRUE);
        }
}



void init_n100(void)
{
 YAP_UserBackCutCPredicate("n100", start_n100, continue_n100, NULL, 1, 1);
}

 /* the next solution

http://www.dcc.fc.up.pt/~michel/TABD/implbdd.pdf

YAP
load_foreign_files(['aula3'],[],init_n100).   -> carrega arquivo SO
n100(X).   -> executa funcao
: -> ponto virgula para carregar o prximo via backtrack
findall(X,n100(X),L).  -> fidl all X atraves da funcoa n100(X), e guardando o resultado na variavel L
*/
