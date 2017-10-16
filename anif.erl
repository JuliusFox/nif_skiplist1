-module(anif).
-author("fox").

%% API
-export([
  init/0, new/0,
  free/1,
  insert/3, delete/3, update/4,
  to_list/1,
  range/3, range_with_score/3, range_by_score/3,
  index_of_score/2, at/2,
  size/1,
  te/0]).

init() ->
  erlang:load_nif("./nif_skiplist", 0).

new() ->
  erlang:nif_error(undef).

free(_List) ->
  erlang:nif_error(undef).

insert(_List, _Score, _Value) ->
  erlang:nif_error(undef).

delete(_List, _Score, _Value) ->
  erlang:nif_error(undef).

update(_List, _Score, _Value, _OldScore) ->
  erlang:nif_error(undef).

to_list(_List) ->
  erlang:nif_error(undef).

size(_List) ->
  erlang:nif_error(undef).

index_of_score(_List, _Score) ->
  erlang:nif_error(undef).

at(_List, _Index) ->
  erlang:nif_error(undef).

range(_List, _Start, _Len) ->
  erlang:nif_error(undef).

range_with_score(_List, _Start, _Len) ->
  erlang:nif_error(undef).

range_by_score(_List, _Score1, _Score2) ->
  erlang:nif_error(undef).

te() ->
  init(),
  A = new(),
  insert(A, 10, 10),
  insert(A, 5, 5),
  insert(A, 8, 8),
  insert(A, 1, 1),
  insert(A, 4, 4),
  insert(A, 7, 7),
  io:format("~p~n", [to_list(A)]),

  delete(A, 7, 7),
  io:format("~p~n", [to_list(A)]),

  io:format("get=~p~n", [anif:index_of_score(A, 5)]),
  io:format("get=~p~n", [anif:index_of_score(A, 6)]),
  io:format("get=~p~n", [anif:index_of_score(A, 15)]),

  io:format("at=~p~n", [at(A, 1)]),
  io:format("at=~p~n", [at(A, 5)]),
  io:format("at=~p~n", [at(A, 6)]),

  io:format("range=~p~n", [range(A, 1, 10)]),
  io:format("range=~p~n", [range(A, 2, 3)]),
  io:format("range=~p~n", [range(A, 8, 10)]),

  io:format("range_with_score=~p~n", [range_with_score(A, 1, 10)]),
  io:format("range_with_score=~p~n", [range_with_score(A, 2, 3)]),
  io:format("range_with_score=~p~n", [range_with_score(A, 8, 10)]),

  erlang:statistics(wall_clock),
  loop_test(30*1000, A),
  {_, Time} = erlang:statistics(wall_clock),
  io:format("time=~p~n", [Time]),

  io:format("range_by_score=~p~n", [range_by_score(A, -10, 10)]),
  io:format("range_by_score=~p~n", [range_by_score(A, 2, 3)]),
  io:format("range_by_score=~p~n", [range_by_score(A, 101, 10)]),

  free(A).

loop_test(0, _) ->
  ok;
loop_test(N, L) ->
  case rand:uniform(500) < 300 of
    true ->
      insert(L, rand:uniform(100), rand:uniform(20));
    _ ->
      delete(L, rand:uniform(100), rand:uniform(20))
  end,
  loop_test(N - 1, L).


