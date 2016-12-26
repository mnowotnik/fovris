
        std::vector<Module> modules = sp.parse("module p:\
                    domains:\
                        literal state.\
                    relations:\
                        parent(literal,literal).\
                        ancestor(literal,literal).\
                    rules:\
                        ancestor(X,Y):- ancestor(X,Z),parent(Z,Y).\
                        ancestor(X,Y):- parent(X,Y).\
                    facts:\
                        parent(alice,bob).\
                        parent(alice,bill).\
                        parent(bob,carol).\
                        parent(carol,dennis).\
                        parent(carol,david).\
                end.");

        Module m("p",
                    {
                        {"parent",{TermType::Id,TermType::Id}},
                        {"ancestor",{TermType::Id,TermType::Id}}
                    },{
                        {
                            {"ancestor",{"X","Y"}},{{"ancestor",{"X","Z"}},{"parent",{"Z","Y"}}}
                        },{
                            {"ancestor",{"X","Y"}},{{"parent",{"X","Y"}}}
                        }
                    },{
                        {"parent",{"alice","bob"}},
                        {"parent",{"alice","bill"}},
                        {"parent",{"bob","carol"}},
                        {"parent",{"carol","dennis"}},
                        {"parent",{"carol","david"}}
                    }
                );
