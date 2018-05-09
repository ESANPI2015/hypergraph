#include "HypergraphDB.hpp"
#include <fstream>
#include <iostream>

HypergraphDB::HypergraphDB(const std::string& dir)
{
    int ret;
    repo = NULL;
    std::cout << "Init libgit2\n";
    git_libgit2_init();
    // Try to open git
    std::cout << "Open repository at " << dir << std::endl;
    ret = git_repository_open(&repo, dir.c_str());
    if (ret || !repo)
    {
        std::cout << "Init repository at " << dir << std::endl;
        //Try to create git
        ret = git_repository_init(&repo, dir.c_str(), 0);
        if (ret || !repo)
        {
            std::cout << "Init of repository at " << dir << " failed" << std::endl;
            return;
        }
        //Create a README.md
        std::ofstream readme;
        readme.open(dir+"/README.md");
        if (!readme.good())
        {
            std::cout << "Failed to open " << dir+"/README.md" << std::endl;
            return;
        }
        readme << "# Hypergraph DB\n";
        readme.close();
        // Make initial commit
        // First, get index and add new file
        git_index *index;
        git_repository_index(&index, repo);
        git_index_add_bypath(index, "README.md");
        git_oid treeId;
        git_index_write_tree(&treeId, index);
        git_index_free(index);
        // Create a signature
        git_signature *me;
        git_signature_now(&me, "HypergraphDB::HypergraphDB", "moritz.schilling@dfki.de");
        // Create commit
        git_oid commitId;
        git_tree *tree;
        git_tree_lookup(&tree, repo, &treeId);
        git_commit_create_v(&commitId, repo, "HEAD", me, me, NULL, "HypergraphDB::HypergraphDB initial commit", tree, 0);
        git_tree_free(tree);
        git_signature_free(me);
    }
    // We have created or opened a repo :)
    std::cout << "Repository at " << dir << " ready to be used" << std::endl;
}

HypergraphDB::~HypergraphDB()
{
    if (repo)
    {
        std::cout << "Close repository\n";
        git_repository_free(repo);
    }
    std::cout << "Shutdown libgit2\n";
    git_libgit2_shutdown();
}

Hypergraph HypergraphDB::create(const std::string& name)
{
    return Hypergraph();
}

Hypergraph HypergraphDB::open(const std::string& name)
{
    return Hypergraph();
}

bool HypergraphDB::commit(const std::string& name, const Hypergraph& graph)
{
    return false;
}
