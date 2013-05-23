/*
 *	Copyright (c) 2013, Synapticon GmbH
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions are met: 
 *
 *	1. Redistributions of source code must retain the above copyright notice, this
 *	   list of conditions and the following disclaimer. 
 *	2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution. 
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The views and conclusions contained in the software and documentation are those
 *	of the authors and should not be interpreted as representing official policies, 
 *	either expressed or implied, of the FreeBSD Project.
 *
 *  parse_mode_xml.h created by Christian Holl
 */

#ifndef SEBS_PARSE_MODE_XML_H_
#define SEBS_PARSE_MODE_XML_H_

#define SEBS_PARSE_XML_MAX_DEPTH 20

#include <rosc/sebs_parse_fw/sebs_parser_frame.h>
#include <rosc/string_res/msg_strings.h>
#include <rosc/sebs_parse_fw/std_modules/sebs_parse_seekstring.h>

typedef enum
{
	SEBS_PARSE_XML_STATE_INIT,
	SEBS_PARSE_XML_STATE_ROOT,
	SEBS_PARSE_XML_STATE_INNER,
	SEBS_PARSE_XML_STATE_TAG_START,
	SEBS_PARSE_XML_STATE_TAG,
	SEBS_PARSE_XML_STATE_CLOSE_TAG_START,
	SEBS_PARSE_XML_STATE_EXPECT_SELFCLOSE_TAG_END,
	SEBS_PARSE_XML_STATE_CDATA,
	SEBS_PARSE_XML_STATE_CDATA_START,
	SEBS_PARSE_XML_STATE_CDATA_EXPECT_OPEN_BRACKET,
	SEBS_PARSE_XML_STATE_CDATA_FIRST_CLOSE_BRACKET,
	SEBS_PARSE_XML_STATE_CDATA_SECOND_CLOSE_BRACKET,
	SEBS_PARSE_XML_STATE_QMTAG_EXPECT_CLOSE,
	SEBS_PARSE_XML_STATE_COMMENT_START_1ST_DASH,
	SEBS_PARSE_XML_STATE_COMMENT,
	SEBS_PARSE_XML_STATE_COMMENT_END_1ST_DASH,
	SEBS_PARSE_XML_STATE_COMMENT_END_2ND_DASH,
	SEBS_PARSE_XML_STATE_ATTRIBUTE_WAIT_EQUAL,
	SEBS_PARSE_XML_STATE_ATTRIBUTE_WAIT_VAL_BOUND,
	SEBS_PARSE_XML_STATE_ATTRIBUTE_INSIDE_APOSTROPHE_ATTRIB,
	SEBS_PARSE_XML_STATE_ATTRIBUTE_INSIDE_QUOTES_ATTRIB,
	SEBS_PARSE_XML_STATE_INNER_CONTENT,
} sebs_parse_xml_state_t;

typedef enum
{
	SEBS_PARSE_XML_SUBSTATE_NONE,
	SEBS_PARSE_XML_SUBSTATE_TAG_ID,
	SEBS_PARSE_XML_SUBSTATE_ATTRIBUTE_ID,
	SEBS_PARSE_XML_SUBSTATE_COMMENT_END_1ST_DASH,
	SEBS_PARSE_XML_SUBSTATE_COMMENT_END_2ND_DASH,
	SEBS_PARSE_XML_SUBSTATE_CDATA_TAG_STRING,
} sebs_parse_xml_sub_state_t;

typedef enum
{
	SEBS_PARSE_XML_EVENT_NONE = SEBS_PARSE_EVENT_NONE, //!< means that there is no event atm.
	SEBS_PARSE_XML_EVENT_ERROR_DEPTH, //!< means that the depth of the current xml document is too depth to be handled
	SEBS_PARSE_XML_EVENT_ERROR_MALFORMED, //!< means that something inside the xml document is wrong
	SEBS_PARSE_XML_EVENT_TAG, //!< means that a xml tag is found and the parser is now after the tag string
	SEBS_PARSE_XML_EVENT_INSIDE_TAG, //!< means that the parser entered the tag and is now after the '>'
	SEBS_PARSE_XML_EVENT_HANDLER_CALLED_SUBMODE_FINISHED, //!< means that the submode called from the handler is finished
	SEBS_PARSE_XML_EVENT_CONTENT_START, //!< means that the parser found content inside a tag and now is at its beginning
} sebs_parse_xml_event_t;

typedef enum
{
	SEBS_PARSE_XML_TAG_TYPE_NORMAL,
	SEBS_PARSE_XML_TAG_TYPE_CLOSE,
	SEBS_PARSE_XML_TAG_TYPE_QUESTION_MARK,
	SEBS_PARSE_XML_TAG_TYPE_EXCLAMATION_MARK,
	SEBS_PARSE_XML_TAG_TYPE_COMMENT,
	SEBS_PARSE_XML_TAG_TYPE_CDATA,
} sebs_parse_xml_tag_type_t;

#define SEBS_PARSE_KNOWN_TAGS __RPC_XML_TAG_STRINGS(SEBS_PARSE_XML) //!< Tag Enum Entries
#define SEBS_PARSE_KNOWN_TAGS_ARRAY rpc_xml_tag_strings //!< Tag String Array
#define SEBS_PARSE_KNOWN_TAGS_ARRAY_LEN RPC_XML_TAG_STRINGS_LEN //!< Tag String Array Length
#define SEBS_PARSE_KNOWN_ATTRIBUTES __RPC_XML_ATTRIBUTE_STRINGS(SEBS_PARSE_XML) //!< Attribute Enum Entries
#define SEBS_PARSE_KNOWN_ATTRIBUTES_ARRAY rpc_xml_attribute_strings //!< Attribute String Array
#define SEBS_PARSE_KNOWN_ATTRIBUTES_ARRAY_LEN RPC_XML_ATTRIBUTE_STRINGS_LEN //!< Attribute String Array Length

/**
 * XML Tags
 * This enum contains the number for each known tag
 */
typedef enum
{
	SEBS_PARSE_XML_TAG_UNKNOWN = -2,
	SEBS_PARSE_XML_TAG_NONE = -1,
	SEBS_PARSE_KNOWN_TAGS
	//!< imports all string numbers from autogenerated file
} parse_xml_tags_t;

/**
 * XML Attributes
 */
typedef enum
{
	SEBS_PARSE_XML_ATTRIBUTE_UNKNOWN = -2,
	SEBS_PARSE_XML_ATTRIBUTE_NONE = -1,
	SEBS_PARSE_KNOWN_ATTRIBUTES
	//!< imports all string numbers from autogenerated file
} parse_xml_attrib_t;

typedef struct
{
	/**
	 * Parser data for advanced parser function
	 * which uses events and other subfunctions.
	 */
	sebs_parser_data_t* parser_data;

	uint16_t depth;	//!< depth stores the depth of the current location of the parser
	uint32_t processed_bytes; //!< currently processed bytes of the xml part (updated after each chunk)
	uint16_t current_tag; 	//!< current_tag contains the current known xml tag
	sebs_parse_xml_tag_type_t tag_type;   //!< normal, ! or ? tag.
	parse_xml_attrib_t attribute; //!< shows the attribute if inside an attribute otherwise -1
	sebs_parse_xml_state_t state; //!< contains the current state of the xml parser
	sebs_parse_xml_sub_state_t substate; //!< gives additional state information
	parse_xml_tags_t tags[SEBS_PARSE_XML_MAX_DEPTH]; //!< array for checking tag validity

	union
	{
		sebs_parse_copy2buffer_data_t copy2buffer;
		sebs_parse_numberparse_result_t numberparse;
		sebs_parse_seekstring_data_t seekstring;
	} std_func_data;
} sebs_parse_xml_data_t;

#ifndef FORCE_INLINE
bool sebs_parse_xml(char **buf, int32_t *len, sebs_parse_xml_data_t *data);
#endif

#endif /* SEBS_PARSE_MODE_XML_H_ */
