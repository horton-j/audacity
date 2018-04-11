/**********************************************************************

  Audacity: A Digital Audio Editor

  AudacityCommand.h

  James Crook

**********************************************************************/

#ifndef __AUDACITY_COMMAND__
#define __AUDACITY_COMMAND__

#include "../Audacity.h"
#include "../MemoryX.h"
#include <set>

#include "../MemoryX.h"
#include <wx/bmpbuttn.h>
#include <wx/defs.h>
#include <wx/dynarray.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/tglbtn.h>
#include <wx/event.h> // for idle event.

class wxCheckBox;
class wxChoice;
class wxListBox;
class wxWindow;

#include "../Experimental.h"
#include "../SampleFormat.h"
#include "../SelectedRegion.h"
#include "../Shuttle.h"
#include "../Internat.h"
#include "../widgets/ProgressDialog.h"
#include "../include/audacity/IdentInterface.h"
#include "../include/audacity/EffectAutomationParameters.h" // for command automation

#include "../Track.h"
#include "../effects/Effect.h"
#include "../Registrar.h"

class ShuttleGui;

#define BUILTIN_GENERIC_COMMAND_PREFIX wxT("Built-in AudacityCommand: ")

class AudacityProject;
class CommandContext;


class AUDACITY_DLL_API AudacityCommand /* not final */ : public wxEvtHandler,
                                public CommandDefinitionInterface
{
 public:
   //std::unique_ptr<CommandOutputTargets> mOutput;
   //CommandOutputTargets * mOutput;
 public:
   AudacityCommand();
   virtual ~AudacityCommand();

   // IdentInterface implementation

   //These four can be defaulted....
   wxString GetPath() override;
   wxString GetName() override;
   wxString GetVendor() override;
   wxString GetVersion() override;
   //  virtual wxString GetFamily();

   //These two must be implemented by instances.
   virtual wxString GetSymbol() override
   { wxFAIL_MSG( "Implement a Symbol for this command");return "FAIL";};
   virtual wxString GetDescription() override
   {wxFAIL_MSG( "Implement a Description for this command");return "FAIL";};

   // Name of page in the Audacity alpha manual
   virtual wxString ManualPage(){ return wxEmptyString;};
   virtual bool IsBatchProcessing(){ return mIsBatch;}
   virtual void SetBatchProcessing(bool start){ mIsBatch = start;};
   
   virtual bool Apply(const CommandContext & WXUNUSED(context) ) {return false;}; 
   virtual bool Apply(); // redirects to the command context version.

   bool ShowInterface(wxWindow *parent, bool forceModal = false);
   virtual void SetHostUI(EffectUIHostInterface * WXUNUSED(host)){;};

   bool PopulateUI(wxWindow *parent);
   wxDialog *CreateUI(wxWindow *parent, AudacityCommand *client);

   virtual bool GetAutomationParameters(wxString & parms);
   virtual bool SetAutomationParameters(const wxString & parms);

   bool DoAudacityCommand(wxWindow *parent, const CommandContext & context,bool shouldPrompt = true);

   // Nonvirtual
   // Display a message box, using effect's (translated) name as the prefix
   // for the title.
   enum : long { DefaultMessageBoxStyle = wxOK | wxCENTRE };
   int MessageBox(const wxString& message,
                  long style = DefaultMessageBoxStyle,
                  const wxString& titleStr = wxString{});

//
// protected virtual methods
//
// Each subclass of AudacityCommand overrides one or more of these methods to
// do its processing.
//
//protected:

   // Called once each time an effect is called.  Perform any initialization;
   // make sure that the command can be performed and
   // return false otherwise
   virtual bool Init();

   // If necessary, open a dialog to get parameters from the user.
   // This method will not always be called (for example if a user
   // repeats a command using 'repeat last command') but if it is called, 
   // it will be called after Init.
   virtual bool PromptUser(wxWindow *parent);

   // Check whether command should be skipped
   // Typically this is only useful in automation, for example
   // detecting that zero noise reduction is to be done,
   // or that normalisation is being done without Dc bias shift
   // or amplitude modification
   virtual bool CheckWhetherSkipAudacityCommand() { return false; }

   // clean up any temporary memory, needed only per invocation of the
   // effect, after either successful or failed or exception-aborted processing.
   // Invoked inside a "finally" block so it must be no-throw.
   virtual void End(){;};
   virtual void PopulateOrExchange(ShuttleGui & WXUNUSED(S)){return;};
   virtual bool TransferDataToWindow();
   virtual bool TransferDataFromWindow();

protected:

   ProgressDialog *mProgress; // Temporary pointer, NOT deleted in destructor.
   // UI
   wxDialog       *mUIDialog;
   wxWindow       *mUIParent;
   int             mUIResultID;

private:
   bool mIsBatch;
   bool mUIDebug;
   bool mNeedsInit;
};


// Base dialog for command dialog.
class AUDACITY_DLL_API AudacityCommandDialog /* not final */ : public wxDialogWrapper
{
public:
   // constructors and destructors
   AudacityCommandDialog(wxWindow * parent,
                const wxString & title,
                AudacityCommand * pCommand,
                int type = 0,
                int flags = wxDEFAULT_DIALOG_STYLE,
                int additionalButtons = 0);

   bool Init();// always returns true.  The bool is for the future...

   bool TransferDataToWindow() override;
   bool TransferDataFromWindow() override;
   bool Validate() override;

   virtual void PopulateOrExchange(ShuttleGui & S);
   virtual void OnOk(wxCommandEvent & evt);
   virtual void OnCancel(wxCommandEvent & evt);
   virtual void OnHelp(wxCommandEvent & evt);

private:
   int mType;
   int mAdditionalButtons;
   AudacityCommand * mpCommand;

   DECLARE_EVENT_TABLE()
   wxDECLARE_NO_COPY_CLASS(AudacityCommandDialog);
};



#endif