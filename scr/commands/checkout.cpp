#include "commands/checkout.h"
#include "core/gitObject.h"
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
#include "dataStructure/cmpMap.h"
#include "dataStructure/Ntree.h"
#include <iostream>
using namespace std;

void checkoutCommandExe(int argc, char *argv[])
// pit checkout 67efef23fef23ef2f3ef23fe23e32f4
{

    if (argc == 3)
    {
        if ((!Repository::stagedChangesExist() || !Repository::unstagedChangesExist()))
        {
            string inputHashOrBr = argv[3 - 1];
            // compare working-head
            vector<string> allBranches = Repository::getAllBranches();
            bool isaBranch = false;
            for (auto br : allBranches)
                if (inputHashOrBr == br)
                    isaBranch = true;
            string inputHash = inputHashOrBr;
            if (!storedObjectExistsWithHash(inputHashOrBr) && isaBranch)
            {
                string cHash = Repository::BranchPointToHashOrNothing(inputHashOrBr); // pass branch to get hash
                if (cHash != "")
                {
                    inputHash = cHash;
                    Repository::setHEAD(inputHashOrBr);
                }
            }

            if (storedObjectExistsWithHash(inputHash))
            {

                string rawFileContents = readFileWithStoredObjectHash(inputHash);
                GitObject gObj(rawFileContents); // deserilze constructor called
                if (gObj.objectType == Commit)
                {
                    CommitObject cObj(rawFileContents);
                    string treeRawFileContents = readFileWithStoredObjectHash(cObj.treeHash);
                    TreeObject tObj(treeRawFileContents);
                    map<path, treeEntry> flattenTree = Repository::FlattenTreeObject(tObj);
                    cmpMap<path, treeEntry, path> cmptEPa;

                    for (auto it : flattenTree) // tree enrty fill
                    {
                        treeEntry tE = it.second;
                        cmptEPa.addVal1(it.first, tE);
                        cout << it.first << "  " << tE.name << "  " << tE.hash << endl;
                    }

                    // StagingIndex::save();
                    traverseDirectory(Repository::project_absolute, [&](const fs::path &relPath)
                                      { cmptEPa.addVal2(relPath, relPath); });

                    cout << "Comparison Table: " << endl;
                    for(auto it: cmptEPa.umap)
                    {
                        cout << it.first << " : ";
                        cout << " Val1Exists:(lastCommit) " << it.second.val1Exists() << " , ";
                        cout << " Val2Exists:(WR) " << it.second.val2Exists() << endl;
                    }


                    for (auto it : cmptEPa.umap)
                    {

                        path filePath = it.first;
                        treeEntry tE = it.second.getVal1();
                        cout << filePath;

                        cout << "  is in TrackedFile " << StagingIndex::isTrackedFile(filePath);
                        // if (StagingIndex::isTrackedFile(filePath)   ) //
                        {
                            if (!it.second.val2Exists()) // not exist in WR , but exist in commit
                            {
                                cout << "         Write file" << endl;
                                string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                                BlobObject bObj(blobRawFileContents); // deserilized constructor called
                                writeFile(filePath, bObj.contents);   // Re-write
                            }
                            else if (!it.second.val1Exists() && StagingIndex::isTrackedFile(filePath)) // so in commit, it does'nt exist so del from here if a tracked file
                            {
                                cout << "Delete file" << endl;
                                deleteFile(filePath);
                            }
                            Cmp_Status cmpStatus = Repository::WorkingDirCommitComp(filePath, tE.hash);
                            if (cmpStatus == CMP_DIFFER) // if the file is diff in WR or not Exist in WR
                            {
                                cout << "         Contents Differ" << endl;
                                string blobRawFileContents = readFileWithStoredObjectHash(tE.hash);
                                BlobObject bObj(blobRawFileContents); // deserilized constructor called
                                writeFile(filePath, bObj.contents);   // Re-write
                            }
                            else if (cmpStatus == CMP_SAME)
                            {
                                
                                // do nothing
                            }
                        }
                    }

                    // Updating Index
                    StagingIndex::indexEntries.clear(); // we have to update index also
                    StagingIndex::save();
                    for (auto it : flattenTree)
                    {
                        treeEntry tE = it.second;
                        indexEntry iE(tE.mode, tE.hash, "0", it.first.string());
                        StagingIndex::addEntry(iE);
                    }
                    StagingIndex::save();
                }
                else
                {
                    cout << "Hash doesn't represent a commit to checkout." << endl;
                }
            }
        }

        else
        {
            cout << "error:Your local changes to the following files would be overwritten by checkout :" << endl;
            cout << "(use \" git status \" to check these local changes) " << endl;
            cout << "Please Commit your changes before checkout." << endl;
            cout << "Aborting" << endl;
        }
    }
    else
    {
        cout << "Invalid Params for checkout command." << endl;
    }
}
