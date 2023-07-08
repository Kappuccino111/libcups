//
// Notification routines for CUPS.
//
// Copyright © 2021-2023 by OpenPrinting.
// Copyright © 2007-2013 by Apple Inc.
// Copyright © 2005-2006 by Easy Software Products.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#include "cups-private.h"


//
// 'cupsLocalizeNotifySubject()' - Return the localized subject for the given notification message.
//
// This function returns a localized subject string for the given notification
// message.  The returned string must be freed by the caller using `free`.
//

char *					// O - Subject string or `NULL`
cupsLocalizeNotifySubject(
    cups_lang_t *lang,			// I - Language data
    ipp_t       *event)			// I - Event data
{
  char			buffer[1024];	// Subject buffer
  const char		*prefix,	// Prefix on subject
			*state;		// Printer/job state string
  ipp_attribute_t	*job_id,	// notify-job-id
			*job_name,	// job-name
			*job_state,	// job-state
			*printer_name,	// printer-name
			*printer_state,	// printer-state
			*printer_uri,	// notify-printer-uri
			*subscribed;	// notify-subscribed-event


  // Range check input...
  if (!event || !lang)
    return (NULL);

  // Get the required attributes...
  job_id        = ippFindAttribute(event, "notify-job-id", IPP_TAG_INTEGER);
  job_name      = ippFindAttribute(event, "job-name", IPP_TAG_NAME);
  job_state     = ippFindAttribute(event, "job-state", IPP_TAG_ENUM);
  printer_name  = ippFindAttribute(event, "printer-name", IPP_TAG_NAME);
  printer_state = ippFindAttribute(event, "printer-state", IPP_TAG_ENUM);
  printer_uri   = ippFindAttribute(event, "notify-printer-uri", IPP_TAG_URI);
  subscribed    = ippFindAttribute(event, "notify-subscribed-event", IPP_TAG_KEYWORD);


  if (job_id && printer_name && printer_uri && job_state)
  {
    // Job event...
    prefix = cupsLangGetString(lang, _("Print Job:"));

    switch (job_state->values[0].integer)
    {
      case IPP_JSTATE_PENDING :
          state = cupsLangGetString(lang, _("pending"));
	  break;
      case IPP_JSTATE_HELD :
          state = cupsLangGetString(lang, _("held"));
	  break;
      case IPP_JSTATE_PROCESSING :
          state = cupsLangGetString(lang, _("processing"));
	  break;
      case IPP_JSTATE_STOPPED :
          state = cupsLangGetString(lang, _("stopped"));
	  break;
      case IPP_JSTATE_CANCELED :
          state = cupsLangGetString(lang, _("canceled"));
	  break;
      case IPP_JSTATE_ABORTED :
          state = cupsLangGetString(lang, _("aborted"));
	  break;
      case IPP_JSTATE_COMPLETED :
          state = cupsLangGetString(lang, _("completed"));
	  break;
      default :
          state = cupsLangGetString(lang, _("unknown"));
	  break;
    }

    snprintf(buffer, sizeof(buffer), "%s %s-%d (%s) %s", prefix, printer_name->values[0].string.text, job_id->values[0].integer, job_name ? job_name->values[0].string.text : cupsLangGetString(lang, _("untitled")), state);
  }
  else if (printer_uri && printer_name && printer_state)
  {
    // Printer event...
    prefix = cupsLangGetString(lang, _("Printer:"));

    switch (printer_state->values[0].integer)
    {
      case IPP_PSTATE_IDLE :
          state = cupsLangGetString(lang, _("idle"));
	  break;
      case IPP_PSTATE_PROCESSING :
          state = cupsLangGetString(lang, _("processing"));
	  break;
      case IPP_PSTATE_STOPPED :
          state = cupsLangGetString(lang, _("stopped"));
	  break;
      default :
          state = cupsLangGetString(lang, _("unknown"));
	  break;
    }

    snprintf(buffer, sizeof(buffer), "%s %s %s", prefix, printer_name->values[0].string.text, state);
  }
  else if (subscribed)
  {
    cupsCopyString(buffer, subscribed->values[0].string.text, sizeof(buffer));
  }
  else
  {
    return (NULL);
  }

  // Duplicate and return the subject string...
  return (strdup(buffer));
}


//
// 'cupsLocalizeNotifyText()' - Return the localized text for the given notification message.
//
// This function returns a localized text string for the given notification
// message.  The returned string must be freed by the caller using `free`.
//

char *					// O - Message text or `NULL`
cupsLocalizeNotifyText(
    cups_lang_t *lang,			// I - Language data
    ipp_t       *event)			// I - Event data
{
  ipp_attribute_t	*notify_text;	// notify-text


  // Range check input...
  if (!event || !lang)
    return (NULL);

  // Get the notify-text attribute from the server...
  if ((notify_text = ippFindAttribute(event, "notify-text", IPP_TAG_TEXT)) == NULL)
    return (NULL);

  // Return a copy...
  return (strdup(notify_text->values[0].string.text));
}
