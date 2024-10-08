% The rule for predicate names: Predicate names ending with '_declared' are
% definitely defined by the user (from Clingo's perspective).

%-----------------------------------------------------------------------------
% Declaration
%-----------------------------------------------------------------------------

% do not warn if generated program contains none of these.
% 'error' is intentionally left undefined within the logic program.
#defined error/4.
#defined error/5.

%-----------------------------------------------------------------------------
% Version semantics
%-----------------------------------------------------------------------------

% If a package is selected (selected_pkg == true), then choose one version
% (selected_pkg_version) from all available versions of that package
% (version_declared).
1 { selected_pkg_version(PkgType, PkgName, PkgVersion) : version_declared(PkgType, PkgName, PkgVersion, PkgWeight) } 1
  :- selected_pkg(PkgType, PkgName).

% Get the weight of the selected version of the selected package. This rule
% declares that only one selected_pkg_weight fact can be derived based on the
% conditions provided.
1 { selected_pkg_weight(PkgType, PkgName, PkgWeight) : version_declared(PkgType, PkgName, PkgVersion, PkgWeight) } 1
  :- selected_pkg_version(PkgType, PkgName, PkgVersion),
     selected_pkg(PkgType, PkgName).

% 'selected_pkg_version' imply new 'selected_pkg'.
selected_pkg(PkgType, PkgName)
  :- selected_pkg_version(PkgType, PkgName, PkgVersion).

%-----------------------------------------------------------------------------
% Dependency semantics
%-----------------------------------------------------------------------------

% It's an error when depending on a version which is not declared.
error(0, "Depends on an undeclared version '@{2}' of '{0}::{1}'", PkgType, PkgName, PkgVersion)
  :- not version_declared(PkgType, PkgName, PkgVersion, _),
     depends_on_declared(_, _, PkgType, PkgName, PkgVersion).

% Select only 1 version for the dependency package.
1 { depends_on(PkgType, PkgName, PkgVersion, DepType, DepName, DepVersion) : depends_on_declared(PkgType, PkgName, PkgVersion, DepType, DepName, DepVersion) } 1
  :- selected_pkg_version(PkgType, PkgName, PkgVersion),
     depends_on_declared(PkgType, PkgName, PkgVersion, DepType, DepName, _).

% 'depends_on' imply new 'selected_pkg_version'.
selected_pkg_version(DepType, DepName, DepVersion)
  :- depends_on(PkgType, PkgName, _, DepType, DepName, DepVersion).

% Every root_declared must be a selected_pkg.
selected_pkg(PkgType, PkgName) :- root_declared(PkgType, PkgName).

% @{
% All nodes in the graph must be reachable from some root.
needed(PkgType, PkgName) :- root_declared(PkgType, PkgName).

needed(DepType, DepName)
  :- needed(PkgType, PkgName),
     depends_on(PkgType, PkgName, _, DepType, DepName, _).

error(0, "'{0}::{1}' is not a valid dependency for any package in the DAG", PkgType, PkgName)
  :- selected_pkg(PkgType, PkgName),
     not needed(PkgType, PkgName).
% @}

%-----------------------------------------------------------------------------
% Domain heuristic
%-----------------------------------------------------------------------------

% @{
% Enable clingo to solve selected_pkg_version/2 and selected_pkg_weight/2 first.
#heuristic selected_pkg_version(PkgType, PkgName, PkgVersion) : version_declared(PkgType, PkgName, PkgVersion, _), selected_pkg(PkgType, PkgName). [10, true]

#heuristic selected_pkg_weight(PkgType, PkgName, _) : version_declared(PkgType, PkgName, PkgVersion, _), selected_pkg(PkgType, PkgName). [10, true]
% @}

%-----------------------------------------------------------------------------
% Optimization
%-----------------------------------------------------------------------------

%
#minimize {
  % What we want to do is, choosing the one with the least weight of the root
  % packages if the total version weight of two subgraphs are equal. In other
  % words, select the highest version of the root packages first.

  % First minimize the weight of packages declared as root packages.
  PkgWeight@998,PkgType,PkgName : root_declared(PkgType, PkgName), selected_pkg_weight(PkgType, PkgName, PkgWeight);

  % Then minimize the weight of all selected packages,regardless of whether they are root packages.
  PkgWeight@999,PkgType,PkgName : selected_pkg_weight(PkgType, PkgName, PkgWeight)
}.

%-----------------------------------------------------------------------------
% Optimization to avoid errors
%-----------------------------------------------------------------------------

% Some errors are handled as rules instead of constraints because it allows us
% to explain why something failed. Here we optimize HEAVILY against the facts
% generated by those rules.

%{
% The following line said that to minimize the count of 'error(Priority, Msg)'
% in each 'Priority'.
#minimize{ 1000@1000+Priority,Msg,Arg1,Arg2: error(Priority, Msg, Arg1, Arg2) }.
#minimize{ 1000@1000+Priority,Msg,Arg1,Arg2,Arg3: error(Priority, Msg, Arg1, Arg2, Arg3) }.
%}
