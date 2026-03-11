## GRAMMAR
```

Program       ::= {DefFunc}+ '\0'

DefFunc       ::= Name L_BRACKET FuncParams R_BRACKET Block
FuncParams    ::= {Name {COMMA Name}*} | [NONE]

CallFunc      ::= Name L_BRACKET FuncArgs R_BRACKET
FuncArgs      ::= {Expression {COMMA Expression}*} | [NONE]

Block         ::= BLOCK_OPEN {Line}+ BLOCK_END
Line          ::= {IfSection | WhileSection | ElseSection | Return | Assignment | Init } LINE_END

WhileSection  ::= WHILE L_BRACKET Expression R_BRACKET {LINE_END}* Block
IfSection     ::= IF    L_BRACKET Expression R_BRACKET {LINE_END}* Block
Init          ::= Name INIT Expression
Assignment    ::= Name ASSIGN Expression
Return        ::= RETURN Expression

Expression    ::= OperAND      { [OR] OperAND }*
OperAND       ::= OperCompare  { [AND] OperCompare }*
OperCompare   ::= OperADD_SUB  { [EQ NEQ GT GT_EQ LT LT_EQ] OperADD_SUB}
OperADD_SUB   ::= OperMUL_DIV  { [ADD SUB] OperMUL_DIV }*
OperMUL_DIV   ::= OperDEG      { [MUL DIV] OperDEG }*
OperDEG       ::= Elementary   { [DEG] Elementary }*
Elementary    ::= FuncCall | Name | Num | L_BRACKET Expression R_BRACKET
               
Name          ::= [a-zA-Z_][a-zA-Z_]+
Num           ::= [0-9]+

```
