#include<bits/stdc++.h>

typedef unsigned long long int ull;
typedef std::vector < unsigned long long int > vull;
typedef std::pair < unsigned long long int, unsigned long long int > pull;
typedef std::vector < bool > vb;
typedef std::vector < std::vector < unsigned long long int > > vvull;
typedef std::vector < std::pair < unsigned long long int, unsigned long long int > > vpull;
typedef std::set < std::pair < unsigned long long int, unsigned long long int > > spull;
typedef std::set < unsigned long long int > sull;
typedef std::vector < std::string > vs;
typedef std::vector < char > vc;

#define mp make_pair
#define ff first
#define ss second
#define pb push_back

class dsu {

    ull _sz;
    vull _parent;
    vull _size;

public:

    dsu(const ull& sz = 0):_sz(sz),_parent(sz),_size(sz,1) {
        for(ull ii = 0; ii < sz; ++ii) _parent[ii] = ii;
    }

    const ull operator ()(const ull& v) {
        if(v == _parent[v]) return v;
        return ( _parent[v] = this-> operator()(_parent[v]) );
    }

    void operator ()(const ull& a, const ull& b) {
        ull a1 = this-> operator()(a), b1 = this-> operator()(b);
        if(a1 != b1) {
        if(_size[a1] < _size[b1]) std::swap(a1, b1);
            _parent[b1] = a1;
            _size[a1] += _size[b1];
        }
    }
};

class Graph {

    ull _vrt;
    ull _edg;
    vvull _adjlist;

public:

    Graph(const ull& vrt = 0): _vrt(vrt), _edg(0), _adjlist(vrt, vull()) {}

    void addEdge(const ull& u, const ull& v) {
        _edg++;
        _adjlist[u].pb(v);
    }

    const bool checkit(const spull& unmarked, const pull& x, const char& alph, const vs& states) const {
        for(const ull& y: _adjlist[x.ff]) {
            bool okay = true;
            for(const ull& z: _adjlist[x.ss])
                okay &= (unmarked.count( std::mp(std::min(y,z), std::max(y,z)) ) == 0);
            if(okay) {
                std::cout << "Found " << states[x.ff] << " -- " << alph << " --> " <<  states[y]  << " ..... ";
                return true;
            }
        }
        for(const ull& y: _adjlist[x.ss]) {
            bool okay = true;
            for(const ull& z: _adjlist[x.ff])
                okay &= (unmarked.count( std::mp(std::min(y,z), std::max(y,z)) ) == 0);
            if(okay) {
                std::cout << "Found " << states[x.ss] << " -- " << alph << " --> " <<  states[y]  << " ..... ";
                return true;
            }
        }
        return false;
    }

    void print() const {
        for(ull ii = 0; ii < _vrt; ++ii) {
            std::cout << ii << " : ";
            for(const ull& x: _adjlist[ii])
                std::cout << x << ' ';
            std::cout << '\n';
        }
    }

    void minimize(dsu& D, Graph& G, std::map <ull, ull>& mapit) const {
        spull minimaledges;
        for(ull ii = 0; ii < _vrt; ++ii) {
            for(const ull& x : _adjlist[ii]) {
                if(ii == x || (ii != x && D(ii) != D(x)) )
                    minimaledges.insert(std::mp(D(ii), D(x)));
            }
        }
        for(const pull& x : minimaledges)
            G.addEdge(mapit[x.ff], mapit[x.ss]);
    }
};

class NFA {

    ull _alphabetsize;
    ull _nstates;
    ull _ntransitions;
    std::vector < Graph > _transition_graphs;
    sull _final;

public:

    NFA(const ull& alphabetsize = 0, const ull& nstates = 0, const ull& ntransitions = 0) : _alphabetsize(alphabetsize), _nstates(nstates), _ntransitions(0) , _transition_graphs(alphabetsize, Graph(nstates)) {}

    void addTransition(const ull& u, const ull& s, const ull& v) {
        _ntransitions++;
        _transition_graphs[s].addEdge(u, v);
    }

    void addFinal(const ull& f) {
        _final.insert(f);
    }

    const bool checkit(const spull& unmarked, const pull& x, const vc& alph, const vs& states) const {
        for(ull ii = 0; ii < _alphabetsize; ++ii) {
            if(_transition_graphs[ii].checkit(unmarked, x, alph[ii], states))
                return true;
        }
        return false;
    }

    const NFA minimize(const vc& alph, const vs& states) const {
        spull unmarked;
        for(ull ii = 0; ii < _nstates; ++ii)
        {
            for(ull jj = ii; jj < _nstates; ++jj)
            {
                if((_final.count(ii) == 0 && _final.count(jj) == 0) || (_final.count(ii) != 0 && _final.count(jj) != 0))
                    unmarked.insert(std::mp(ii, jj));
            }
        }

        while(1)
        {
            std::cout << "\nUnmarked Pairs:\n";
            for(const pull& x : unmarked)
                std::cout << states[x.ff] << ' ' << states[x.ss] << '\n';
            std::cout << '\n';

            spull unmarkedt = unmarked;
            for(const pull& x : unmarked)
            {
                if(checkit(unmarkedt, x, alph, states))
                {
                    unmarkedt.erase(std::mp(x.ff, x.ss));
                    std::cout << "Marking " << states[x.ff] << ' ' << states[x.ss] << '\n';
                }
            }

            if(unmarkedt.size() == unmarked.size())
                break;
            unmarked = unmarkedt;
        }

        dsu D(_nstates);
        for(const pull& x : unmarked)
            D(x.ff, x.ss);

        vvull minimal(_nstates);
        for(ull ii = 0; ii < _nstates; ++ii)
            minimal[D(ii)].pb(ii);

        ull n(0);
        std::map < ull, ull> mapit;
        std::cout << "Equivalent Classes:\n";
        for(ull ii = 0; ii < _nstates; ++ii)
        {
            if(minimal[ii].size() != 0)
            {
                std::cout << n << " : ";
                mapit[ii] = n;
                n++;
                for(const ull& x: minimal[ii])
                    std::cout << states[x] << ' ';
                std::cout << '\n';
            }
        }
        std::cout << '\n';

        NFA M(_alphabetsize, n);

        for(ull ii = 0; ii < _alphabetsize; ++ii)
            _transition_graphs[ii].minimize(D, M._transition_graphs[ii], mapit);

        for(ull ii = 0; ii < _nstates; ++ii) {
            if(_final.count(ii) != 0)
                M._final.insert(mapit[D(ii)]);
        }

        return M;
    }

    void print() const {
        std::cout << "Alphabet Size: " << _alphabetsize << '\n';
        std::cout << "Number of States: " << _nstates << '\n';
        std::cout << "Transitions:\n\n";
        for(ull ii = 0; ii < _alphabetsize; ++ii) {
            std::cout << "On reading " << ii << '\n';
            _transition_graphs[ii].print();
            std::cout << '\n';
        }
        std::cout << "Final States:\n";
        for(const ull& x : _final)
            std::cout << x << ' ';
        std::cout << '\n' << std::flush;
    }

};

using namespace std;

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    ull s;
    cout << "\nAlphabet Size: " << flush;
    cin >> s;
    vc alph(s);
    map < char, ull > invalph;
    cout << "\nAlphabet:\n(Enter all the distinct elements of the alphabet continuously without white space)\n" << flush;
    for(ull ii = 0; ii < s; ++ii) {
        cin >> alph[ii];
        if(invalph.count(alph[ii]) != 0)
        {
            cerr << "\nRepeated Alphabet\n";
            return -1;
        }
        invalph[alph[ii]] = ii;
    }

    ull n;
    cout << "\nNumber of States: " << flush;
    cin >> n;
    vs states(n);
    map < string, ull > invstates;
    cout << "\nStates:\n(Enter a distinct string for every state in a new line)\n" << flush;
    for(ull ii = 0; ii < n; ++ii) {
        cin >> states[ii];
        if(invstates.count(states[ii]) != 0)
        {
            cerr << "\nRepeated State\n";
            return -1;
        }
        invstates[states[ii]] = ii;
    }

    ull m;
    cout << "\nNumber of Transitions: " << flush;
    cin >> m;
    NFA N(s, n);
    cout << "\nTransitions:\n(Enter in the format: p a q , where there is a transitions from p to q while reading a)\n" << flush;
    for(ull ii = 0; ii < m; ++ii) {
        string p, q;
        char ch;
        cin >> p >> ch >> q;
        N.addTransition(invstates[p], invalph[ch], invstates[q]);
    }

    ull f;
    cout << "\nNumber of final states: " << flush;
    cin >> f;
    cout << "\nFinal states:\n" << flush;
    for(ull ii = 0; ii < f; ++ii)
    {
        string temp;
        cin >> temp;
        N.addFinal(invstates[temp]);
    }

    cout << "\n\nExecuting the NFA Minimization Algorithm ...\n\n";
    NFA M = N.minimize(alph, states);

    cout << "\n\nPrinting the minimal NFA ....\n\n";
    M.print();

    return 0;
}
