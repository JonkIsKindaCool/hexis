#include "hxs_data.h"

void Hxs_freeObjField(HxsObjField *field){
    free(field->name);

    Hxs_free_Expr(field->value);

    free(field);
}
void Hxs_freeFuncArg(HxsFuncArg* arg);
void Hxs_freeAnonField(HxsAnonField* arg);
void Hxs_freePosition(HxsPosition *pos);