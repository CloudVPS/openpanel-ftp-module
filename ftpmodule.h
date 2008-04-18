// --------------------------------------------------------------------------
// OpenPanel - The Open Source Control Panel
// Copyright (c) 2006-2007 PanelSix
//
// This software and its source code are subject to version 2 of the
// GNU General Public License. Please be aware that use of the OpenPanel
// and PanelSix trademarks and the IconBase.com iconset may be subject
// to additional restrictions. For more information on these restrictions
// and for a copy of version 2 of the GNU General Public License, please
// visit the Legal and Privacy Information section of the OpenPanel
// website on http://www.openpanel.com/
// --------------------------------------------------------------------------

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
