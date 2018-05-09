#include "HypergraphDB.hpp"
#include "HypergraphYAML.hpp"
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
    if ((ret < 0) || !repo)
    {
        std::cout << "Init repository at " << dir << std::endl;
        //Try to create git
        ret = git_repository_init(&repo, dir.c_str(), 0);
        if ((ret < 0) || !repo)
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
        if (git_index_add_bypath(index, "README.md") < 0)
        {
            std::cout << "Failed to add README.md\n";
            git_index_free(index);
            return;
        }
        git_index_write(index);
        git_oid treeId;
        git_index_write_tree(&treeId, index);
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
        git_index_free(index);
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

Hypergraph HypergraphDB::open(const std::string& name)
{
    return YAML::LoadFile(std::string(git_repository_workdir(repo))+"/"+name).as<Hypergraph>();
}

bool HypergraphDB::commit(const std::string& name, const Hypergraph& graph)
{
    // Store the graph
    std::ofstream hg;
    std::string filename(std::string(git_repository_workdir(repo))+"/"+name);
    hg.open(filename);
    if (!hg.good())
    {
        std::cout << "Failed to open " << filename << std::endl;
        return false;
    }
    hg << YAML::StringFrom(graph) << std::endl;
    hg.close();

    // Lookup head
    git_oid headId;
    git_reference_name_to_id(&headId, repo, "HEAD");
    git_commit *headCommit;
    if (git_commit_lookup(&headCommit, repo, &headId))
    {
        std::cout << "Could not resolve HEAD\n";
        return false;
    }
    const git_commit *parents[] = {headCommit};
    // Add new file 
    git_index *index;
    git_repository_index(&index, repo);
    if (git_index_add_bypath(index, name.c_str()) < 0)
    {
        std::cout << "Could not add " << name << "\n";
        git_index_free(index);
        git_commit_free(headCommit);
        return false;
    }
    git_index_write(index);
    git_oid treeId;
    git_index_write_tree(&treeId, index);
    // Create a signature
    git_signature *me;
    git_signature_now(&me, "HypergraphDB::commit", "moritz.schilling@dfki.de");
    // Create commit
    git_oid commitId;
    git_tree *tree;
    git_tree_lookup(&tree, repo, &treeId);
    int ret = git_commit_create(&commitId, repo, "HEAD", me, me, NULL, "Update DB", tree, 1, parents);
    git_tree_free(tree);
    git_signature_free(me);
    git_index_free(index);
    git_commit_free(headCommit);

    return ret ? false : true;
}
