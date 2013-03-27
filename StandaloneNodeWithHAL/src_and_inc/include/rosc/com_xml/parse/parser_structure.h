
#ifndef PARSER_STRUCTURE_H_
#define PARSER_STRUCTURE_H_

#include <rosc/rosc.h>
#include <rosc/com_xml/parse/sub/parseurl.h>
#include <rosc/string_res/msg_strings.h>
#include <rosc/com_xml/parse/sub/seekstring.h>
#include <rosc/com_xml/parse/sub/copy2buffer.h>
#include <rosc/com_xml/parse/sub/parseurl.h>
#include <rosc/com_xml/parse/sub/numberparse.h>
#include <rosc/com_xml/parse/sub/skipuntilchar.h>
#include <rosc/com_xml/parse/sub/skipwholemessage.h>
/**
 * This enum contains the state for the xml definition check
 */
typedef enum
{
	XML_DEF_SEEK_LT, 		//!< XML_DEF_SEEK_LT means that parser is currently searching for "<"
	XML_DEF_SEEK_QM0,		//!< XML_DEF_SEEK_QM0 means that parser awaits a question mark as next char
	XML_DEF_SEEK_X,  		//!< XML_DEF_SEEK_X means that parser awaits a x now
	XML_DEF_SEEK_M,  		//!< XML_DEF_SEEK_M means that parser awaits a m now
	XML_DEF_SEEK_L,  		//!< XML_DEF_SEEK_L means that parser awaits a l now
	XML_DEF_SEEK_QM1,		//!< XML_DEF_SEEK_QM1 means that parser awaits another ? now ignoring all other chars inside the def tag
	XML_DEF_SEEK_GT,      	//!< XML_DEF_SEEK_GT means that parser awaits ">"
	XML_PARSE,       	  	//!< XML_PARSE means that the parser is currently parsing XML
	XML_PARSE_INSIDE_TAG, 	//!< XML_PARSE_INSIDE_TAG means that the parser is parsing XML an currently inside a tag definition
}parse_xml_state_t;

/**
 * This enum contains the number for each known tag
 */
typedef enum
{
	__RPC_XML_TAG_STRINGS(XML) //!< imports all string numbers from autogenerated file
}parse_xml_tags_t;


/**
 * This enum contains the number of each known http method
 */
typedef enum
{
	__HTTP_METHODS(ENUM) //!< imports all string numbers from autogenerated file
}parse_http_methods_t;


/**
 * Contains the header descriptors
 */
typedef enum
{
	__HTTP_HEADER_DESCRIPTORS(PARSE_HTTP)//!< __HTTP_HEADER_DESCRIPTORS inserts auto generated list of header descriptors
}parse_http_desc_t;

/**
 *	States of HTTP parser
 */
typedef enum
{
	PARSE_HTTP_STATE_METHSTR_BEGIN, //!<PARSE_HTTP_STATE_METHSTR_BEGIN - Init with seeking method string
	PARSE_HTTP_STATE_METHSTR_METHODSTR,//!< PARSE_HTTP_STATE_METHSTR_METHODSTR -Checking method
	PARSE_HTTP_STATE_METHSTR_BCKSLSH0, //!< PARSE_HTTP_STATE_METHSTR_BCKSLSH0 - Check backslash
	PARSE_HTTP_STATE_METHSTR_TARGET,   //!< PARSE_HTTP_STATE_METHSTR_TARGET - Check if target available
	PARSE_HTTP_STATE_METHSTR_HTTP,     //!< PARSE_HTTP_STATE_METHSTR_HTTP - Parse HTTP/1.1
	PARSE_HTTP_STATE_METHSTR_LF,        //!< PARSE_HTTP_STATE_METHSTR_LF - wait for line feed
	PARSE_HTTP_STATE_DESCRIPTOR,       //!< PARSE_HTTP_STATE_DESCRIPTOR - seek descriptor inside string array
	PARSE_HTTP_STATE_FIELD,           //!< PARSE_HTTTP_STATE_FIELD - parse state parsed descripter, decide if known field

	PARSE_HTTP_STATE_GET_FIELD_VALUE,	//! PARSE_HTTP_STATE_FIELD_VALUE - Setup field parsing...
	PARSE_HTTP_STATE_CONTENT_TYPE,	//! PARSE_HTTP_STATE_CONTENT_TYPE - Parse the content type field and check for (un)supported type(s)
	PARSE_HTTP_STATE_CONTENT_LENGTH,    //! PARSE_HTTP_STATE_CONTENT_LENGTH - Parse the length of the incoming stuff

//	PARSE_HTTP_STATE_FIELD_CHECK_HANDLER_CMD, //!< PARSE_HTTP_STATE_FIELD_CHECK_HANDLER_CMD check if handler is interested in the field
}parse_http_state_t;

/**
 * This enum contains the two main modes of the parser
 */
typedef enum
{
	PARSE_MODE_HEADER,//!< PARSE_MODE_HEADER is when parsing the header
	PARSE_MODE_XML,   //!< PARSE_MODE_XML is when parsing the xml
}parse_mode_t;

/**
 * This enum contains the values for different commands from the handler function back to the parser
 */
typedef enum
{
	PARSE_NONE_GO_AHEAD,  //!< PARSE_NONE_GO_AHEAD means that the parser just should go on
	PARSE_SEEKSTRING,     //!< PARSE_SEEKSTRING Seek a string inside a string array
	PARSE_NUMBERPARSE,    //!< PARSE_NUMBERPARSE Parse a unsigned integer number
	PARSE_COPY2BUFFER,    //!< PARSE_COPY2BUFFER Copy a string to the buffer
	PARSE_SKIPWHOLEMESSAGE//!< PARSE_SKIPWHOLEMESSAGE Truncate whole message (for errors)
}parse_ctrl_t;

/**
 * This enum contains the states for the current submode
 */
typedef enum
{
	PARSE_SUBMODE_NONE,            //!< PARSE_SUBMODE_NONE means that no submode is active
	PARSE_SUBMODE_SEEKSTRING,      //!< PARSE_SUBMODE_SEEKSTRING means that the parser currently seeks a string inside an array
	PARSE_SUBMODE_NUMBERPARSE,     //!< PARSE_SUBMODE_NUMBERPARSE means that currently the number parser is in operation
	PARSE_SUBMODE_COPY2BUFFER,     //!< PARSE_SUBMODE_COPY2BUFFER means that currently a string is copied into another location
	PARSE_SUBMODE_SKIPUNTILCHAR,   //!< PARSE_SUBMODE_SKIPUNTILCHAR means that currently everything is skipped till a specified char is received
	PARSE_SUBMODE_SKIPWHOLEMESSAGE,//!< PARSE_SUBMODE_SKIPWHOLEMESSAGE means that the parser is currently skipping the whole message
}parse_submode_t;


/**
 * Values for the state of the current submode
 */
typedef enum
{
	PARSE_SUBMODE_INIT,    //!< PARSE_SUBMODE_INIT means it was not initialized yet
	PARSE_SUBMODE_RUNNING, //!< PARSE_SUBMODE_RUNNING means that the submode is running
	PARSE_SUBMODE_FINISHED,//!< PARSE_SUBMODE_FINISHED means that the submode has finshed
}parse_submode_state_t;


/**
 * Contains events for the handler for it to know what currently happend
 */
typedef enum
{
	PARSE_EVENT_ERROR_CONTENT_LENGTH_TOO_LONG=-5,
	PARSE_EVENT_ERROR_CONTENT_LENGTH=-4,
	PARSE_EVENT_ERROR_404=-3,
	PARSE_EVENT_ERROR_HTTP_BAD_REQUEST=-2,
	PARSE_EVENT_ERROR_HTTP_METHOD=-1,
	PARSE_EVENT_NONE=0,
	PARSE_EVENT_HTTP_METHOD_PARSED,
	PARSE_EVENT_HTTP_TARGET_PARSED,
	PARSE_EVENT_HTTP_HEADER_FIELD,
}parse_event_t;






/**
 * Definition for handler function type
 */
typedef struct parse_act_t
{
	void (*handler_fkt)(void *); //!< This function handles the parser events
	void *handler_data_storage;	//!< This is a pointer for different handlers data storage
	uint32_t content_length; //!< xml_length stores the length parsed from the http header
	uint32_t content_curChr; //!< xml_curChr keeps the current xml char number
	parse_mode_t mode; //!< mode saves contains the current main mode, xml or http header parsing
	parse_submode_t submode; //!< submode is the current sub mode the parser is using

	parse_submode_state_t submode_state; //!< is one when submode is finished
	int16_t submode_result;	//!< contains the result code from each submode when finished
	parse_ctrl_t ctrl_command; //!< ctrl_command contains current command from the handler to the parser
	parse_event_t event; //!< tells the handler function what currently has happened.

	/**
	 * The mode_data union stores the http and the xml data inside the same memory location.
	 * This is used for saving memory space, because the variables are not used the same time.
	 */
	union
	{
		struct
		{
			parse_http_state_t state; //!< state contains the current state of the http parser
			parse_http_desc_t descriptor; //!< descriptor contains the current known descriptor
		}http;

		struct
		{
			uint16_t depth;	//!< depth stores the depth of the current location of the parser
			parse_xml_tags_t current_tag; //!< current_tag contains the current known xml tag
			parse_xml_state_t state; //!< contains the current state of the xml parser
		}xml;
	}mode_data;

	/**
	 * submode_data contains the data for the different submodes
	 */
	union
	{
		struct
		{
			char **stringlist;	//!< The stringlist
			char * endchrs;		//!< endchrs is a list of characters (string) that will end the seek like "<" when reading inside tags
			uint16_t stringlist_len; //!< Length of the stringlist to be checked for the string
			uint16_t curChrPos;	//!< The char number since the start of seekString
			uint16_t fit_min;	//!< The beginning of the range with possibly matching strings
			uint16_t fit_max;	//!< The end of the range with possibly matching strings
			char separator; //!< the separator which was found
		}seekString;

		struct
		{
			uint8_t cur_place; //!< cur_place stores the number of the current figure of the number
			uint8_t figure_max; //!< figure_max stores the maximum of figures to be allowed
			uint32_t number; //!< contains the parsed number
		}numberParse;

		struct
		{
			char* buffer; //!< buffer points to the place where the chars have to be stored in memory.
			uint16_t cur_pos; //!< cur_pos stores the amount of already copied chars.
			uint16_t max_len; //!< max_len is the maximum length to be used for the buffer.
			char* endChrs; //!< chrs which will mark the end of the string
		}copy2Buffer;

		struct
		{
			char* chrs; //!< chrs to skip/ to stop skip, according to negative
			bool negative; //!< negative, if true the chars inside chrs will stop skipping
			char fndchr; //!< the chr which was found
		}skipUntilChr;

		struct
		{
			char hostname_ip_char[__HOSTNAME_MAX_LEN__]; //!< The text form of the hostname/IP
			uint16_t curLen; //!< curLen The current size of the copied chars
			uint16_t IPv6[8];//!< storage for an IPv6 address
			uint8_t IPv4[4]; //!< storage for an IPv4 address or the resolving end of IPv6
			parse_url_match_t what; //!< what specifies what kind of address is given
		}parseURL;

	}submode_data;

}parse_act_t;


/**
 * Definition for handler function type
 */
typedef void (*parse_handler_fct)(parse_act_t *pact);
typedef void (*parse_handler_fct_cast)(void *pact);




#endif /* PARSER_STRUCTURE_H_ */
