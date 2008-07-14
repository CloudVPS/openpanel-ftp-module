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

#include <moduleapp.h>
#include "ftpmodule.h"

#include <grace/file.h>
#include <grace/filesystem.h>
#include <grace/exception.h>


using namespace moderr;

APPOBJECT(ftpmodule);

//  =========================================================================
/// Main method.
//  =========================================================================
int ftpmodule::main (void)
{
	string conferr;

	// Add configuration watcher
	conf.addwatcher ("config", &ftpmodule::confSystem);


   // Load will fail if watchers did not valiate.
    if (! conf.load ("openpanel.module.ftp", conferr))
    {   
        ferr.printf ("%% Error loading configuration: %s\n", conferr.str());
        return 1;
    }
    
    value &sessiondat = data["OpenCORE:Session"];
    string vhostname;
    
    if (data.exists ("Domain:Vhost"))
    	vhostname = data["Domain:Vhost"]["id"];
    
	caseselector (command)
	{
		incaseof ("create") :
			createAccount (data[classid], vhostname);
			authd.installfile ("pureftpd.passwd", "/etc/pure-ftpd");
			authd.installfile ("pureftpd.pdb", "/etc/pure-ftpd");
			authd.quit ();
			return 0;
			
		incaseof ("update") :
			updateAccount (metaid, data[classid]["password"]);
			authd.installfile ("pureftpd.passwd", "/etc/pure-ftpd");
			authd.installfile ("pureftpd.pdb", "/etc/pure-ftpd");
			authd.quit ();
			return 0;
			
		incaseof ("delete") :
			deleteAccount (metaid);
			authd.installfile ("pureftpd.passwd", "/etc/pure-ftpd");
			authd.installfile ("pureftpd.pdb", "/etc/pure-ftpd");
			authd.quit ();
			return 0;
		 	
		incaseof ("validate") : 
			break;
			
		defaultcase:
			sendresult (err_command, "Unsupported command");
			return 0;
	}

	authd.quit ();
	sendresult (moderr::ok, "");
	return 0;
}

void ftpmodule::createAccount (const value &data,
							  const string &vhost)
{
	if (! data.exists ("password"))
	{
		sendresult (moderr::err_module, "Missing password");
		return;
	}
	
	if (! data.attribexists ("owner"))
	{
		sendresult (moderr::err_module, "Missing owner attribute");
		return;
	}
	
	value pwv;
	pwv = kernel.userdb.getpwnam (data("owner").sval());
	if (! pwv.count())
	{
		sendresult (moderr::err_module, "Unknown user");
		return;
	}
	
	uid_t uid;
	gid_t gid;
	string homedir;
	string ftpusername;
	string pwhash;
	
	uid = pwv["uid"].uval();
	gid = pwv["gid"].uval();
	homedir = pwv["home"];
	ftpusername = data["id"];
	pwhash = data["password"];
	
	if (vhost.strlen())
	{
		homedir.printf ("/web/%s", vhost.str());
	}
	
	if ( (! uid) || (! gid) )
	{
		sendresult (moderr::err_module, "Invalid uid/gid for user");
		return;
	}
	
	if (! ftpusername.globcmp ("*@*.*"))
	{
		sendresult (moderr::err_module, "Invalid username");
		return;
	}
	
	if (! pwhash.strlen())
	{
		sendresult (moderr::err_module, "Empty password hash invalid");
		return;
	}
	
	if (! pw.add (ftpusername, pwhash, homedir, uid, gid))
	{
		sendresult (moderr::err_module, "Could not add user");
		return;
	}
	
	sendresult (moderr::ok, "");
}

void ftpmodule::deleteAccount (const string &metaid)
{
    pw.remove (metaid);
	
	sendresult (moderr::ok, "");
}

void ftpmodule::updateAccount (const string &metaid, const string &pwhash)
{
	if (! pw.update (metaid, pwhash))
	{
		sendresult (moderr::err_module, "Could not update user");
		return;
	}
	
	sendresult (moderr::ok, "");
}

//  =========================================================================
//	METHOD: ftpmodule::readconfiguration
//  =========================================================================
bool ftpmodule::readconfiguration (void)
{
	return true;
}

//  =========================================================================
//	METHOD: ftpmodule::writeconfiguration
//  =========================================================================
bool ftpmodule::writeconfiguration (const value &v)
{
	//	======================================================
	//  EXAMPLE ERROR:
	//	string error;						
	//	error.printf ("Error creating database\n");
	//	error.printf ("Code: %i\n", clftp->errorno());
	//	error.printf ("Msg: %s\n", clftp->error().str());
	//	
	//	// Send error
	//	sendresult (err_module, error);
	//	return false;
	//			

	//	======================================================
	//  EXAMPLE ERROR:			
	//	sendresult 
	//		(
	//		err_context, 
	//		"Supported classes are: MySQL:(Database, DBUser, DBUserhost)"
	//		);
	//	return false;


	return true;
}



//  =========================================================================
/// METHOD ::checkconfig
//  =========================================================================
bool ftpmodule::checkconfig (value &v)
{
	// No errors during validation
	return true;
}



//  =========================================================================
/// Configuration watcher for the event log.
//  =========================================================================
bool ftpmodule::confSystem (config::action act, keypath &kp,
                const value &nval, const value &oval)
{
	switch (act)
	{
		case config::isvalid:
			return true;

		case config::create:
			return true;		
	}

	return false;
}

purepw::purepw (void)
{
}

purepw::~purepw (void)
{
}

#define SPATH_PUREFTPD_PASSWD "/var/opencore/conf/staging/FTP/pureftpd.passwd"
#define SPATH_PUREFTPD_NPASSWD "/var/opencore/conf/staging/FTP/pureftp.passwd.new"
#define SPATH_PUREFTPD_PDB "/var/opencore/conf/staging/FTP/pureftpd.pdb"

bool purepw::add (const string &username, const string &pwhash,
				  const string &home, uid_t uid, gid_t gid)
{
	file fold, fnew;
	
	fold.openread (SPATH_PUREFTPD_PASSWD);
	fnew.openwrite (SPATH_PUREFTPD_NPASSWD);
	
	if (! verifymd5pw(pwhash))
	{
        return false;
	}
	
	while (! fold.eof())
	{
		string line;
		string name;
		
		line = fold.gets ();
		if (! line.strlen()) continue;
		
		name = line.copyuntil (":");
		if (name != username)
		{
			if (! fnew.writeln (line))
			{
				fold.close ();
				fnew.close ();
				fs.rm (SPATH_PUREFTPD_NPASSWD);
				return false;
			}
		}
	}
	fold.close ();
	
	if (! fnew.printf ("%s:%s:%u:%u::%s/./::::::::::::\n",
					   username.str(), pwhash.str(), uid, gid, home.str()))
	{
		fnew.close ();
		fs.rm (SPATH_PUREFTPD_NPASSWD);
		return false;
	}
	
	fnew.close();
	if (! fs.mv (SPATH_PUREFTPD_NPASSWD, SPATH_PUREFTPD_PASSWD))
		return false;
	
	string updcmd;
	updcmd.printf ("/usr/bin/pure-pw mkdb " SPATH_PUREFTPD_PDB " -f "
				   SPATH_PUREFTPD_PASSWD);
	
	if (kernel.sh (updcmd))
	{
		return false;
	}
	

	return true;
}

bool purepw::remove (const string &username)
{
	file fold, fnew;
	
	fold.openread (SPATH_PUREFTPD_PASSWD);
	fnew.openwrite (SPATH_PUREFTPD_NPASSWD);
	
	while (! fold.eof())
	{
		string line;
		string name;
		
		line = fold.gets ();
		if (! line.strlen()) continue;
		name = line.copyuntil (":");
		if (name != username)
		{
			if (! fnew.writeln (line))
			{
				fold.close ();
				fnew.close ();
				fs.rm (SPATH_PUREFTPD_NPASSWD);
				return false;
			}
		}
	}
	fold.close ();
	fnew.close ();
	
	if (! fs.mv (SPATH_PUREFTPD_NPASSWD, SPATH_PUREFTPD_PASSWD))
		return false;
	
	string updcmd;
	updcmd.printf ("/usr/bin/pure-pw mkdb " SPATH_PUREFTPD_PDB " -f "
				   SPATH_PUREFTPD_PASSWD);
	
	if (kernel.sh (updcmd))
	{
		return false;
	}
	
	return true;
}

bool purepw::update (const string &username, const string &npasswd)
{
	file fold, fnew;
	
	fold.openread (SPATH_PUREFTPD_PASSWD);
	fnew.openwrite (SPATH_PUREFTPD_NPASSWD);
	
	if (! verifymd5pw(npasswd))
	{
        return false;
	}

	while (! fold.eof())
	{
		string line;
		string name;
		
		line = fold.gets ();
		if (! line.strlen()) continue;
		name = line.copyuntil (":");
		if (name != username)
		{
			if (! fnew.writeln (line))
			{
				fold.close ();
				fnew.close ();
				fs.rm (SPATH_PUREFTPD_NPASSWD);
				return false;
			}
		}
		else
		{
			value fields = strutil::split (line, ':');
			fields[1] = npasswd;
			
			string out;
			foreach (field, fields)
			{
				if (out) out.strcat (":");
				out.strcat (field.sval());
			}
			
			if (! fnew.writeln (out))
			{
				fold.close ();
				fnew.close ();
				fs.rm (SPATH_PUREFTPD_NPASSWD);
				return false;
			}
		}
	}
	
	fold.close ();
	fnew.close ();
	
	if (! fs.mv (SPATH_PUREFTPD_NPASSWD, SPATH_PUREFTPD_PASSWD))
		return false;
	
	string updcmd;
	updcmd.printf ("/usr/bin/pure-pw mkdb " SPATH_PUREFTPD_PDB " -f "
				   SPATH_PUREFTPD_PASSWD);
	
	if (kernel.sh (updcmd))
	{
		return false;
	}

	return true;
}

bool purepw::verifymd5pw(const string &passwd)
{
    static string validmd5pass ("abcdefghijklmnopqrstuvwxyz0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ/.$");
                                
    return passwd.validate(validmd5pass);                                

}