db_import(Pred,Relation,ConAtom):-
    get_value(ConAtom,ConnHandler),
    db_arity(ConnHandler,Relation,Arity),
    atom_concat('SELECT * FROM ', Relation, SQL),
    functor(Clause,Pred, Arity),
    Clause =.. [Pred|Args],
    assert(':-'(Clause,','(db_query(ConnHandler,SQL,RS),db_row(RS,Args)))).


film_actor(A,B):-
    db_query(CONN,'SELECT * FROM film_actor;',X),
    db_row(X,[A,B]).
