#include "../headers/user_search_engine.h"
#include "../headers/user_manager.h"
#include <algorithm>
#include <iostream>
using namespace std;

UserSearchEngine::UserSearchEngine() 
      : usersByID([](const int& a, const int& b) { return a < b; }),
        usersByName([](const string& a, const string& b) { return a < b; }) {
}

UserSearchEngine::~UserSearchEngine() {
}

void UserSearchEngine::migrateFromLinkedList(const LinkedList<User>& userList) 
{
    auto current = userList.head(); 
    while (current != nullptr)
    {
        cout << "check 1" << endl;
        User* u2 = new User(current->data);
        cout << "check 2" << endl;
        bool added = addUser(u2);

        cout << added << endl;
        if (!added) delete u2;  
        current = current->next;
    }
}

bool UserSearchEngine::addUser(User* user) 
{
    if (!user) return false;
    
    int userid = user->userID;
    string username = user->userName;

    if (usersByID.find(userid)) return false;
    if (usersByName.find(username)) return false;

    bool insertedID = usersByID.insert(userid, user);
    if (!insertedID) return false;

    bool insertedName = usersByName.insert(username, user);
    if (!insertedName)
    {
      usersByID.remove(userid);
      return false;
    }
    return true;

}

bool UserSearchEngine::removeUser(int userID) 
{
    auto foundPtr = usersByID.find(userID);      //! V* where V = User* → User**, took me 2 hrs to debug this shit
    if (!foundPtr || !(*foundPtr)) return false;

    User* u = *foundPtr;

    string username = u->userName;
    bool removeID = usersByID.remove(userID);
    bool removeName = usersByName.remove(username);

    if (removeID && !removeName)
    {
        usersByID.insert(userID, u);
        return false;
    }
    if (!removeID && removeName)
    {
        usersByName.insert(username, u);
        return false;
    }
    return removeID && removeName;
}

bool UserSearchEngine::removeUser(const string& username) 
{
    auto foundPtr = usersByName.find(username);
    if (!foundPtr || !(*foundPtr)) return false;

    User* u = *foundPtr;

    int userid = u->userID;
    bool removeName = usersByName.remove(username);
    bool removeID = usersByID.remove(userid);
    if (removeID && !removeName)
    {
        usersByID.insert(userid, u);
        return false;
    }
    if (!removeID && removeName)
    {
        usersByName.insert(username, u);
        return false;
    }
    return removeID && removeName;
}

User* UserSearchEngine::searchByID(int userID) const 
{
    auto ptr = usersByID.find(userID);
    if (!ptr || !(*ptr)) return nullptr;

    return *ptr;
}

User* UserSearchEngine::searchByUsername(const std::string& username) const 
{

    auto ptr = usersByName.find(username);
    if (!ptr || !(*ptr)) return nullptr;

    return *ptr; 
}

std::vector<User*> UserSearchEngine::searchByUsernamePrefix(const string& prefix) const 
{
    vector<User*> usersbyPrefix;
    vector<User*> allusers = getAllUsersSorted(false);
    
    if (prefix == "") return allusers;
    
    for (User* user : allusers)
    {
        string name = user->userName;
        if (name.substr(0, prefix.length()) == prefix)
        {
            usersbyPrefix.push_back(user);
        }
    }
    return usersbyPrefix;

}

std::vector<User*> UserSearchEngine::getUsersInIDRange(int minID, int maxID) const 
{
    vector<User*> usersbyRange;
    if (minID > maxID) return usersbyRange;
    
    vector<User*> allusers = getAllUsersSorted(true);
    for (User* user : allusers)
    {
        int id = user->userID;
        if (id >= minID && id <= maxID)
        {
            usersbyRange.push_back(user);
        }
    }
    return usersbyRange;

}

std::vector<User*> UserSearchEngine::fuzzyUsernameSearch(const string& username, int maxEditDistance) const 
{
    vector<User*> usersbyNAME;
    if (username == "") return usersbyNAME;
    
    vector<User*> allUsers = getAllUsersSorted(false);

    for (User* user: allUsers)
    {
      string name = user->userName;
      int d = calculateEditDistance(name, username);
      if (d <= maxEditDistance)
      {
          usersbyNAME.push_back(user);
      }
    }
    return usersbyNAME;
}

int UserSearchEngine::calculateEditDistance(const string& str1, const string& str2) const 
{
    int m = str1.length();
    int n = str2.length();

    if (n == 0) return m;
    if (m == 0) return n;

    vector<vector<int>> doublevector(m+1, vector<int>(n+1));

    for (int i =0; i <= m; i++)
    {
       doublevector[i][0] = i;
    }
    for (int j =0; j <= n; j++)
    {
       doublevector[0][j] = j;
    }

    for (int i = 1; i <= m; i++)
    {
      for (int j =1; j <= n; j++)
      {
        if (str1[i-1] == str2[j-1])
        {
          doublevector[i][j] = doublevector[i-1][j-1];
        }
        else
        {
          doublevector[i][j] = 1 + min({doublevector[i-1][j], doublevector[i][j-1], doublevector[i-1][j-1]});
        }
      }
    }
    return doublevector[m][n];
}

vector<User*> UserSearchEngine::getAllUsersSorted(bool byID) const
{
    vector<User*> allusers;
    if (byID)
    {
      vector<pair<int, User*>> ordered_list = usersByID.inOrderTraversal();
      for (auto p: ordered_list)
      {
          allusers.push_back(p.second);
      }
    }
    else
    {
      vector<pair<string, User*>> ordered_list = usersByName.inOrderTraversal();
      for (auto p: ordered_list)
      {
          allusers.push_back(p.second);
      }
    }
    return allusers;
}

size_t UserSearchEngine::getTotalUsers() const 
{
  return usersByID.size();
}

void UserSearchEngine::displaySearchStats() const {
}

bool UserSearchEngine::isConsistent() const 
{
    if (usersByID.size() != usersByName.size()) return false;
    vector<pair<int, User*>> byID = usersByID.inOrderTraversal();

    for (auto entry: byID)
    {
      User *user = entry.second;
      if (!user) return false;

      auto foundbyName = usersByName.find(user->userName);
      if (!foundbyName || !(*foundbyName)|| *foundbyName != user)
      {
        return false;
      }
    }
    return true;
}