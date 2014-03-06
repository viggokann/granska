#include "rules.h"
#include "memhandler.h"


/* Symboltabell med mycket enkel implementation som lista (och hashtabell för globala symboler) : */

IdEntry *IdEntry::firstLocalSymbol = NULL;
IdEntry *IdEntry::lastLocalSymbol = NULL;
HashTable<IdEntry> *IdEntry::globalsTable = NULL;
StringBuf IdEntry::stringBuf;
DefObj(HashTable<IdEntry>);
DefObj(IdEntry);

IdEntry::IdEntry(const char *name_) {
  name = stringBuf.NewString(name_);
  type = UndefinedId;
  semtype = NoType;
  next = NULL;
  NewObj();
  
  Handle_memory(this);
}

IdEntry::IdEntry(const char *name_, enum idtype type_, enum semantictype semtype_) { 
  name = stringBuf.NewString(name_);
  IntoGlobalTable(type_);
  semtype = semtype_;
  next = NULL;
  NewObj();

  Handle_memory(this);
}

IdEntry::IdEntry()
{ 
  NewObj();
  Handle_memory(this);
}

IdEntry *IdEntry::LookUp(const char *identifier)
{
  static IdEntry *a = new IdEntry;  // jb: avoid stack allocation
  IdEntry *p;
  for (p = firstLocalSymbol; p != NULL; p = p->next)
    if (strcmp(identifier, p->name) == 0)
      return p;
  a->name = (char*) identifier;
  p = globalsTable->Find(a);
  if (!p)
    p = new IdEntry(identifier);
  return p;
}

void IdEntry::IntoGlobalTable(enum idtype type_) {
  type = type_;
  globalsTable->Insert(this);
}

void IdEntry::IntoLocalTable() {
  type = ElementId;
  if (firstLocalSymbol == NULL) firstLocalSymbol = this;
  else lastLocalSymbol->next = this;
  lastLocalSymbol = this;
}

void IdEntry::NewScope(void) {
  // defined in rules.y
  extern int currentNoOfElement;
  extern int endLeftContext;
  extern int beginRightContext;

  firstLocalSymbol = lastLocalSymbol = NULL;
  currentNoOfElement = 0;
  endLeftContext = beginRightContext = -1;
}

/* rutiner för utskrift av datastrukturen: */

void IdEntry::Print() const {
  if (type == ElementId || type == RuleElementId || (type == AttributeId && semtype == SemFeatureClass))
    { printf("%s", name); return; }
  printf("(%s: ", name);
  switch(type) {
    case UndefinedId: printf("odefinierad (fel!), "); break;
    case MethodId:    printf("metod, "); break;
    case FunctionId:  printf("funktion, "); break;
    case RuleId:      printf("regel %d", u.ruleNo); break;
    case LabelId:     printf("läge vid regel %d", u.ruleNo); break;
    case ElementId:   printf("element"); break;
    case RuleElementId:   printf("hjälpregelelement för %s", u.re.ruleId->name); break;
    case AttributeId: printf("attribut, "); break;
    case ConstantId:  printf("konstant, "); break;
    default: printf("okänd typ hos identifierare, ");
  }
  if (type != RuleId && type != LabelId && type != ElementId
      && type != RuleElementId) {
    switch (semtype) {
	case Boolean:	printf("boolean"); break;
	case Integer:	printf("integer"); break;
	case Real:	printf("real"); break;
	case String:	printf("string"); break;
	case Variable:	printf("variabel"); break;
        case LexVariable:	printf("lex-variabel"); break;
	case Interval:	printf("intervall"); break;
	case Feature:	printf("särdrag"); break;
	case SemFeatureClass:	printf("särdragsklass");
			if (type == AttributeId) printf(" %d", u.featureClass);
			break;
	default: printf("okänd semantisk typ");
    }
  }
  printf(")");
}


