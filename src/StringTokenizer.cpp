#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer(const std::string &str, const std::string &delim)
{
	if ((str.size() == 0) || (delim.size() == 0))
	{
		return;
	}

	_tokenString = str;
	_delimiter = delim;
	_delimiterLen = delim.size();

	/* Remove sequential delimiter */
	std::size_t	currentPosition = 0;
	while(true)
	{
		currentPosition = _tokenString.find(_delimiter,currentPosition);
		if (currentPosition != std::string::npos)
		{
			currentPosition += _delimiterLen;
			while(_tokenString.find(_delimiter, currentPosition) == currentPosition)
			{
				_tokenString.erase(currentPosition, _delimiterLen);
			}
		}
		else
		{
			break ;
		}
	}

	/* Trim leading delimiter */
	if (_tokenString.find(_delimiter, 0) == 0)
	{
		_tokenString.erase(0, _delimiterLen);
	}

	/* Trim ending delimiter */
	if ((currentPosition = _tokenString.rfind(_delimiter)) != std::string::npos)
	{
		if (currentPosition != (_tokenString.size() - _delimiterLen))
		{
			return ;
		}
		_tokenString.erase(_tokenString.size() - _delimiterLen, _delimiterLen);
	}
}

StringTokenizer::~StringTokenizer() {}

bool StringTokenizer::hasMoreTokens()
{
	return (_tokenString.size() > 0);
}

std::string StringTokenizer::remainingString()
{
	return _tokenString;
}

std::size_t StringTokenizer::countTokens()
{
	if (_tokenString.size() > 0)
	{
		std::size_t	count = 0;
		std::size_t	currentPosition = 0;

		while(true)
		{
			currentPosition = _tokenString.find(_delimiter, currentPosition);
			if (currentPosition != std::string::npos)
			{
				++count;
				currentPosition += _delimiterLen;
			}
			else
			{
				break ;
			}
		}
		return ++count;
	}
	return 0;
}

std::size_t StringTokenizer::countTokens(const std::string &separator)
{
	if (_tokenString.size() > 0)
	{
		std::size_t	count = 0;
		std::size_t	currentPosition = 0;

		while(true)
		{
			currentPosition = _tokenString.find(separator, currentPosition);
			if (currentPosition != std::string::npos)
			{
				++count;
				currentPosition += separator.size();
			}
			else
			{
				break ;
			}
		}
		return ++count;
	}
	return 0;
}

std::string StringTokenizer::nextToken()
{
	if (_tokenString.size() == 0)
	{
		return "";
	}

	std::string	token = "";
	std::size_t	delimiterPosition = _tokenString.find(_delimiter, 0);

	if (delimiterPosition != std::string::npos)
	{
		token = _tokenString.substr(0, delimiterPosition);
		_tokenString = _tokenString.substr(delimiterPosition + _delimiterLen,
						_tokenString.size() - delimiterPosition);
	}
	else
	{
		token = _tokenString.substr(0, _tokenString.size());
		_tokenString = "";
	}
	return token;
}

std::string StringTokenizer::nextToken(const std::string& separator)
{
	if (_tokenString.size() == 0)
	{
		return "";
	}

	std::string	token = "";
	std::size_t	delimiterPosition = _tokenString.find(separator, 0);

	if (delimiterPosition != std::string::npos)
	{
		token = _tokenString.substr(0, delimiterPosition);
		_tokenString = _tokenString.substr(delimiterPosition + separator.size(),
						_tokenString.size() - delimiterPosition);
	}
	else
	{
		token = _tokenString.substr(0, _tokenString.size());
		_tokenString = "";
	}

	return token;
}

// std::string StringTokenizer::filterNextToken(const std::string& filterStr)
// {
// 	std::string tmpString    = nextToken();
// 	std::size_t currentPos = 0;

// 	while((currentPos = tmpString.find(filterStr, currentPos)) != std::string::npos)
// 	{
// 		tmpString.erase(currentPos, filterStr.size());
// 	}
//
// 	return tmpString;
// }