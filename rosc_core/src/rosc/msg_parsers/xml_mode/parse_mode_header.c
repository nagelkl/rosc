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
 *  parse_mode_header.c created by Christian Holl
 */

#include <rosc/msg_parsers/xml_mode/parse_mode_header.h>
#include <rosc/msg_parsers/handler/xmlrpc_string_id.h>
#include <rosc/msg_parsers/sub/subs.h>

bool parse_mode_header(char **buf, int32_t *len, parse_act_t *pact)
{
		while(*len>0 && pact->event  == PARSE_EVENT_NONE)
		{
			bool is_field_content=false;
				/*
				 *	Handling substate results
				 */
					switch(pact->mode_data.http.sub_state)
					{

					case PARSE_HTTP_SUB_CHECK_METHOD:
						if(pact->submode_data.seekstring.result>=0)
						{
							DEBUG_PRINT_STR("->METHOD found");
							pact->event=PARSE_EVENT_HTTP_METHOD_PARSED;
							pact->mode_data.http.state=PARSE_HTTP_STATE_REQUEST_ACTION;
						}
						else
						{
							DEBUG_PRINT_STR("ERROR! HTTP Method unknown");
							pact->event=PARSE_EVENT_ERROR_HTTP_METHOD_NOT_ALLOWED;
						}
						break;


						case PARSE_HTTP_SUB_CHECK_ACTION:
							if(pact->submode_data.seekstring.result>=0)
							{
								DEBUG_PRINT_STR("->ACTION found...");
								pact->event=PARSE_EVENT_HTTP_ACTION_PARSED;
								pact->mode_data.http.state=PARSE_HTTP_STATE_REQUEST_HTTP_VER;
							}
							else
							{
								DEBUG_PRINT_STR("ERROR! HTTP Action unknown");
								pact->event=PARSE_EVENT_ERROR_HTTP_NOT_FOUND;
							}
						break;

						case PARSE_HTTP_SUB_CHECK_REQUEST_HTTP_VER:
							if(pact->submode_data.seekstring.result==HTTP_VAL_HTTP1_0 || pact->submode_data.seekstring.result==HTTP_VAL_HTTP1_1 )
							{
								DEBUG_PRINT_STR("Version found...");
								pact->mode_data.http.state=PARSE_HTTP_STATE_HEADLINE_WAIT_END;
							}
							else
							{
								DEBUG_PRINT_STR("ERROR! HTTP Version unknown");
								pact->event=PARSE_EVENT_ERROR_HTTP_VERSION_NOT_SUPPORTED;
							}
							break;


						case PARSE_HTTP_SUB_CHECK_RESPONSE_HTTP_VER:
							if(pact->submode_data.seekstring.result==HTTP_VAL_HTTP1_0 || pact->submode_data.seekstring.result==HTTP_VAL_HTTP1_1 )
							{
								DEBUG_PRINT_STR("Version found...");
								pact->mode_data.http.state=PARSE_HTTP_STATE_RESPONSE_CODE;
							}
							else
							{
								DEBUG_PRINT_STR("ERROR! RESPONSE HTTP Version unknown");
								pact->event=PARSE_EVENT_ERROR_HTTP_VERSION_NOT_SUPPORTED;
							}
							break;

						case PARSE_HTTP_SUB_CHECK_RESPONSE_CODE:
							switch(pact->submode_data.numberparse.result)
							{
								case NUMBERPARSE_ANOTHER_CHAR:
								if(**buf==' ')
								{
									DEBUG_PRINT(INT,"Code of HTTP Response", pact->submode_data.numberparse.number);
									pact->event=PARSE_EVENT_HTTP_RESPONSE_CODE;
									pact->mode_data.http.state=PARSE_HTTP_STATE_RESPONSE_STRING;
								}
								else
								{
									pact->event=PARSE_EVENT_ERROR_HTTP_BAD_RESPONSE;
								}
								break;

								case NUMBERPARSE_ERROR_NONUMBER:
									pact->event=PARSE_EVENT_ERROR_HTTP_BAD_RESPONSE;
								break;

								case NUMBERPARSE_MAX_FIGURES:
									pact->event=PARSE_EVENT_ERROR_HTTP_BAD_RESPONSE;
								break;
							}
							break;

						case PARSE_HTTP_SUB_CHECK_DESCRIPTOR_ID:
							if(pact->submode_data.seekstring.result>0)
							{
								DEBUG_PRINT(INT,"RESULT",pact->submode_data.seekstring.result);
								DEBUG_PRINT(STR,"Descriptor", http_header_descriptors[pact->submode_data.seekstring.result]);
								pact->mode_data.http.descriptor=pact->submode_data.seekstring.result;
							}
							else
							{
								DEBUG_PRINT_STR("Unknown Descriptor...");
							}
							pact->mode_data.http.state=PARSE_HTTP_STATE_DESCRIPTOR_FIELD_SEPARATOR;
							break;
						case PARSE_HTTP_SUB_CHECK_CONTENT_LENGTH:

							switch(pact->submode_data.numberparse.result)
							{
							case NUMBERPARSE_ANOTHER_CHAR:
								if(**buf==' ' || **buf=='\n')
								{
									pact->content_length=pact->submode_data.numberparse.number;
									DEBUG_PRINT(INT,"Content Length is", pact->content_length);
								}
								else
								{
									pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								}
								break;
							case NUMBERPARSE_ERROR_NONUMBER:
								pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								break;
							case NUMBERPARSE_MAX_FIGURES:
								pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								break;

							}

							break;

							case PARSE_HTTP_SUB_CHECK_CONTENT_TYPE:
								if(pact->submode_data.seekstring.result==HTTP_VAL_TEXT_XML)
								{
									DEBUG_PRINT_STR("Found text/xml!");
									pact->mode_data.http.content_type_text_xml_found=true;
								}
								else
								{
									DEBUG_PRINT_STR("Found other");
									if(!pact->mode_data.http.content_type_text_xml_found)
									{
										pact->event=PARSE_EVENT_ERROR_HTTP_CONTENT_TYPE;
									}
								}

								//default chars will lead to reengage seekstring
								//so we need to skip them here
								if(**buf==',' || **buf==';')
								{
									++*buf;
									--*len;
								}
								break;
					default:
						break;
					}

					/*
					 * Check if previous state was a substate
					 */
					if(pact->mode_data.http.sub_state != PARSE_HTTP_SUB_STATE_NONE)
					{
						pact->mode_data.http.sub_state=PARSE_HTTP_SUB_STATE_NONE;
					}
					else
					{
						/*
						 * Parsing
						 */
						switch(**buf)
						{
						case '/':
							switch(pact->mode_data.http.state)
							{
							case PARSE_HTTP_STATE_REQUEST_ACTION:
								pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_ACTION;
								++*buf;
								--*len;
								PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring),http_available_actions,HTTP_AVAILABLE_ACTIONS_LEN," ",true);
								break;

							case PARSE_HTTP_STATE_FIELD:
								is_field_content=true;
								break;


							case PARSE_HTTP_STATE_FIELD_CONTENT:
								break;

							case PARSE_HTTP_STATE_RESPONSE_STRING:
								break;

							default:
								pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								break;
							}
							break;

						case ':':
							switch(pact->mode_data.http.state)
							{

							case PARSE_HTTP_STATE_FIELD_CONTENT:
								break;

							case PARSE_HTTP_STATE_FIELD:
								 is_field_content=true;
										break;

							case PARSE_HTTP_STATE_DESCRIPTOR_FIELD_SEPARATOR:
								pact->mode_data.http.state=PARSE_HTTP_STATE_FIELD;
								break;

							case PARSE_HTTP_STATE_RESPONSE_STRING:
								break;

							default:
								pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								break;

							}
							break;

						case ' ':
							switch(pact->mode_data.http.state)
							{

							case PARSE_HTTP_STATE_FIELD_CONTENT:
								pact->mode_data.http.state=PARSE_HTTP_STATE_FIELD;
								break;

							case PARSE_HTTP_STATE_RESPONSE_STRING:
								break;

							default:
								break;
							}
							break;

						case '\n':
							switch(pact->mode_data.http.state)
							{
							case PARSE_HTTP_STATE_FIELD:
							case PARSE_HTTP_STATE_FIELD_CONTENT:
							case PARSE_HTTP_STATE_HEADLINE_WAIT_END:
							case PARSE_HTTP_STATE_RESPONSE_STRING:
								pact->mode_data.http.state=PARSE_HTTP_STATE_DESCRIPTOR_OR_HEADER_END;
								break;

							case PARSE_HTTP_STATE_DESCRIPTOR_OR_HEADER_END:
								if(pact->content_length<0)
								{
									pact->event=PARSE_EVENT_ERROR_HTTP_LENGTH_REQUIRED;
								}
								pact->submode_state=PARSE_SUBMODE_INIT;
								pact->mode=PARSE_MODE_XML;
								pact->mode_data.xml.state=PARSE_XML_INIT;
								break;





							case PARSE_HTTP_STATE_RESPONSE_CODE:
							case PARSE_HTTP_STATE_DESCRIPTOR_FIELD_SEPARATOR:
								pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
								break;

							default: //TODO ^^ merge with above???
								break;
							}
							break;

						//Any other char
						default:
							switch(pact->mode_data.http.state)///@note Here is a lot of potential for saving memory by reducing setup cause it is always almost the same
							{
								case PARSE_HTTP_STATE_REQUEST_METHOD:
										pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_METHOD;
										PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring),http_methods,HTTP_METHODS_LEN," /\n.",true);
								break;

								case PARSE_HTTP_STATE_REQUEST_HTTP_VER:
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_REQUEST_HTTP_VER;
									PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring),http_header_stdtext, HTTP_HEADER_STDTEXT_LEN," \n",true);
									break;

								case PARSE_HTTP_STATE_RESPONSE_HTTP_VER:
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_RESPONSE_HTTP_VER;
									PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring),http_header_stdtext, HTTP_HEADER_STDTEXT_LEN," \n",true);
									break;


								case PARSE_HTTP_STATE_RESPONSE_CODE:
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_RESPONSE_CODE;
									PARSE_SUBMODE_INIT_NUMBERPARSE(pact->submode,(&pact->submode_data.numberparse),3,false);
									break;

								case PARSE_HTTP_STATE_DESCRIPTOR_OR_HEADER_END:
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_DESCRIPTOR_ID;
									PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring),http_header_descriptors,HTTP_HEADER_DESCRIPTORS_LEN," :",false);
									break;

								case PARSE_HTTP_STATE_FIELD:
									is_field_content=true;
									break;

								default:
									break;
							}
							break;
						}


						//special external state for field content
						//must be external due to it's size and need to be placed in multiple positions
						if(is_field_content)
						{
							DEBUG_PRINT_STR("CONTENT_START");
							pact->mode_data.http.state=PARSE_HTTP_STATE_FIELD_CONTENT;


							switch(pact->mode_data.http.descriptor)
							{

							case HTTP_DESC_CONTENT_ENCODING:
								/* NO ENCODING IS SUPPORTED->ERROR 415*/
								pact->event=PARSE_EVENT_ERROR_HTTP_CONTENT_ENCODING;
								break;


							case HTTP_DESC_CONTENT_LENGTH:
								if(pact->content_length==-1)
								{
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_CONTENT_LENGTH;
									PARSE_SUBMODE_INIT_NUMBERPARSE(pact->submode,(&pact->submode_data.numberparse),4,0);
								}
								else
								{
									pact->event=PARSE_EVENT_ERROR_HTTP_BAD_REQUEST;
									DEBUG_PRINT_STR("CONTENT LENGTH FIELD ERROR");
								}
								break;
							case HTTP_DESC_CONTENT_TYPE:
									pact->mode_data.http.sub_state=PARSE_HTTP_SUB_CHECK_CONTENT_TYPE;
									PARSE_SUBMODE_INIT_SEEKSTRING(pact->submode,(&pact->submode_data.seekstring), http_header_stdtext, HTTP_HEADER_STDTEXT_LEN," ,;\n",true);
								break;


							default:
								pact->event=PARSE_EVENT_HTTP_HEADER_FIELD_CONTENT;
								pact->mode_data.http.state=PARSE_HTTP_STATE_FIELD_CONTENT;
								break;
							}
						}




						if(pact->submode==0 && len>0 && pact->event!=PARSE_EVENT_XML_CONTENT_START)
						{
							++*buf;
							--*len;
						}
						else
						{
							break;
						}
					}
				}

}




