#ifndef __INCLUDE_SRC_ROUTE__
#define __INCLUDE_SRC_ROUTE__

#include <string>
#include <vector>

class Route {

private:
	bool _autoindex;
	std::vector<int> _methods;
	std::string _rootDir;
	std::vector<std::string> _indexPage;
	std::string _uploadPath;
	std::string _redirCode;
	std::string _redirPage;
	std::string _cgiPath;
	std::string _cgiExtension;
	
	bool _autoindexSet;
	bool _indexPageSet;
	bool _rootDirSet;
	bool _methodsSet;
	bool _cgiSet;
	bool _uploadSet;
	bool _redirectionSet;

public:
	Route();
	Route(std::string);
	~Route();

	bool getAutoindex() const;
	std::vector<int> getMethods() const;
	std::string getRootDir() const;
	std::string getIndexPage() const;
	std::string getUploadPath() const;
	std::string getRedirCode() const;
	std::string getRedirPage() const;
	std::string getCgiPath() const;
	std::string getCgiExtension() const;

	bool hasAutoindex() const;
	bool hasMethods() const;
	bool hasRoot() const;
	bool hasIndexPage() const;
	bool hasRedir() const;
	bool hasUpload() const;
	bool hasCgi() const;

};

#endif // !__INCLUDE_SRC_ROUTE__