// This file is part of OpenPanel - The Open Source Control Panel
// OpenPanel is free software: you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation, using version 3 of the License.
//
// Please note that use of the OpenPanel trademark may be subject to additional 
// restrictions. For more information, please visit the Legal Information 
// section of the OpenPanel website on http://www.openpanel.com/

#ifndef _ftpmodule_H
#define _ftpmodule_H 1

#include <moduleapp.h>
#include <grace/system.h>
#include <grace/configdb.h>

typedef configdb<class ftpmodule> appconfig;

class purepw
{
public:
			 purepw (void);
			~purepw (void);
			
	bool	 add (const string &username, const string &pwhash,
				  const string &home, uid_t uid, gid_t gid);
	bool	 remove (const string &username);
	bool	 update (const string &username, const string &pwhash);
	bool     verifymd5pw (const string &);
};

//  -------------------------------------------------------------------------
/// Main application class.
//  -------------------------------------------------------------------------
class ftpmodule : public moduleapp
{
public:
				 ftpmodule (void) :
					moduleapp ("openpanel.module.ftp"),
					conf (this)
				 {
				 }
			 
				~ftpmodule (void)
				 {
				 }

	int			 main (void);

				 // virtual from moduleapp
	void		 onsendresult ()
				 {
				 }

protected:

	appconfig	 conf;			///< Modules configuration data
	purepw		 pw;
	
	void		 createAccount (const value &, const string &);
	void		 updateAccount (const string &, const string &);
	void		 deleteAccount (const string &);

				 //	 =============================================
				 /// Configuration handler 
				 //	 =============================================
	bool    	 confSystem (config::action act, keypath &path, 
						  	 const value &nval, const value &oval);	
	
				 //	=============================================
				 /// validate the given configuration, this
				 /// will always be executed at setup of 
				 /// every call
				 /// \return true on ok / false on failure
				 //	=============================================
	bool		 checkconfig (value &v);	
	
				 //	=============================================
				 /// Used to read existing unix configuration
				 /// files
				 //	=============================================
	bool		 readconfiguration (void);
		
				 //	=============================================
				 /// Writes a network configuration
				 /// \param v given post data
				 /// \return true on ok / false on failure
				 //	=============================================
	bool 		 writeconfiguration (const value &v);
};

#endif
