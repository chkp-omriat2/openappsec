#include "triggers_section.h"

using namespace std;

USE_DEBUG_FLAG(D_K8S_POLICY);
// LCOV_EXCL_START Reason: no test exist

LogTriggerSection::LogTriggerSection(
    const string &_name,
    const string &_verbosity,
    const string &_extendloggingMinSeverity,
    bool _extendlogging,
    bool _logToAgent,
    bool _logToCef,
    bool _logToCloud,
    bool _logToSyslog,
    bool _responseBody,
    bool _tpDetect,
    bool _tpPrevent,
    bool _webBody,
    bool _webHeaders,
    bool _webRequests,
    bool _webUrlPath,
    bool _webUrlQuery,
    int _cefPortNum,
    const string &_cefIpAddress,
    int _syslogPortNum,
    const string &_syslogIpAddress,
    bool _beautify_logs)
        :
    name(_name),
    verbosity(_verbosity),
    extendloggingMinSeverity(_extendloggingMinSeverity),
    extendlogging(_extendlogging),
    logToAgent(_logToAgent),
    logToCef(_logToCef),
    logToCloud(_logToCloud),
    logToSyslog(_logToSyslog),
    responseBody(_responseBody),
    tpDetect(_tpDetect),
    tpPrevent(_tpPrevent),
    webBody(_webBody),
    webHeaders(_webHeaders),
    webRequests(_webRequests),
    webUrlPath(_webUrlPath),
    webUrlQuery(_webUrlQuery),
    cefPortNum (_cefPortNum),
    cefIpAddress (_cefIpAddress),
    syslogPortNum (_syslogPortNum),
    syslogIpAddress (_syslogIpAddress),
    beautify_logs(_beautify_logs)
{
    try {
        id = to_string(boost::uuids::random_generator()());
        context = "triggerId(" + id + ")";
    } catch (const boost::uuids::entropy_error &e) {
        dbgWarning(D_K8S_POLICY) << "Failed to generate log trigger UUID. Error: " << e.what();
    }
}

void
LogTriggerSection::save(cereal::JSONOutputArchive &out_ar) const
{
    string trigger_type = "log";
    string urlForSyslog = syslogIpAddress + ":" + to_string(syslogPortNum);
    string urlForCef = cefIpAddress + ":" + to_string(cefPortNum);
    out_ar(
        cereal::make_nvp("context",                  context),
        cereal::make_nvp("triggerName",              name),
        cereal::make_nvp("triggerType",              trigger_type),
        cereal::make_nvp("verbosity",                verbosity),
        cereal::make_nvp("acAllow",                  false),
        cereal::make_nvp("acDrop",                   false),
        cereal::make_nvp("complianceViolations",     false),
        cereal::make_nvp("complianceWarnings",       false),
        cereal::make_nvp("extendloggingMinSeverity", extendloggingMinSeverity),
        cereal::make_nvp("extendlogging",            extendlogging),
        cereal::make_nvp("logToAgent",               logToAgent),
        cereal::make_nvp("logToCef",                 logToCef),
        cereal::make_nvp("logToCloud",               logToCloud),
        cereal::make_nvp("logToSyslog",              logToSyslog),
        cereal::make_nvp("responseBody",             responseBody),
        cereal::make_nvp("responseCode",             false),
        cereal::make_nvp("tpDetect",                 tpDetect),
        cereal::make_nvp("tpPrevent",                tpPrevent),
        cereal::make_nvp("webBody",                  webBody),
        cereal::make_nvp("webHeaders",               webHeaders),
        cereal::make_nvp("webRequests",              webRequests),
        cereal::make_nvp("webUrlPath",               webUrlPath),
        cereal::make_nvp("webUrlQuery",              webUrlQuery),
        cereal::make_nvp("urlForSyslog",             urlForSyslog),
        cereal::make_nvp("urlForCef",                urlForCef),
        cereal::make_nvp("formatLoggingOutput",      beautify_logs)
    );
}

const string &
LogTriggerSection::getTriggerId() const
{
    return id;
}

const string &
LogTriggerSection::getTriggerName() const
{
    return name;
}

bool
LogTriggerSection::operator<(const LogTriggerSection &other) const
{
    return getTriggerName() < other.getTriggerName();
}

WebUserResponseTriggerSection::WebUserResponseTriggerSection(
    const string &_name,
    const string &_details_level,
    const string &_response_body,
    int _response_code,
    const string &_response_title)
        :
    name(_name),
    context(),
    details_level(_details_level),
    response_body(_response_body),
    response_title(_response_title),
    response_code(_response_code)
{
    try {
        id = to_string(boost::uuids::random_generator()());
        context = "triggerId(" + id + ")";
    } catch (const boost::uuids::entropy_error &e) {
        dbgWarning(D_K8S_POLICY) << "Failed to generate webUserResponse trigger UUID. Error: " << e.what();
    }
}

void
WebUserResponseTriggerSection::save(cereal::JSONOutputArchive &out_ar) const
{
    out_ar(
        cereal::make_nvp("context",       context),
        cereal::make_nvp("triggerName",   name),
        cereal::make_nvp("details level", details_level),
        cereal::make_nvp("response body", response_body),
        cereal::make_nvp("response code", response_code),
        cereal::make_nvp("response title", response_title)
    );
}

const string &
WebUserResponseTriggerSection::getTriggerId() const
{
    return id;
}

const string &
WebUserResponseTriggerSection::getTriggerName() const
{
    return name;
}

bool
WebUserResponseTriggerSection::operator<(const WebUserResponseTriggerSection &other) const
{
    return getTriggerName() < other.getTriggerName();
}

void
AppSecCustomResponseSpec::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec web user response spec";
    parseAppsecJSONKey<int>("http-response-code", httpResponseCode, archive_in, 403);
    parseAppsecJSONKey<string>("mode", mode, archive_in, "block-page");
    parseAppsecJSONKey<string>("name", name, archive_in);
    if (mode == "block-page") {
        parseAppsecJSONKey<string>(
            "message-body",
            messageBody,
            archive_in,
            "Openappsec's <b>Application Security</b> has detected an attack and blocked it."
        );
        parseAppsecJSONKey<string>(
            "message-title",
            messageTitle,
            archive_in,
            "Attack blocked by web application protection"
        );
    }
}

int
AppSecCustomResponseSpec::getHttpResponseCode() const
{
    return httpResponseCode;
}

const string &
AppSecCustomResponseSpec::getMessageBody() const
{
    return messageBody;
}

const string &
AppSecCustomResponseSpec::getMessageTitle() const
{
    return messageTitle;
}

const string &
AppSecCustomResponseSpec::getMode() const
{
    return mode;
}

const string &
AppSecCustomResponseSpec::getName() const
{
    return name;
}

void
TriggersRulebase::save(cereal::JSONOutputArchive &out_ar) const
{
    out_ar(
        cereal::make_nvp("log",             logTriggers),
        cereal::make_nvp("webUserResponse", webUserResponseTriggers)
    );
}

void
AppsecTriggerAccessControlLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec Trigger - Access Control Logging";
    parseAppsecJSONKey<bool>("allow-events", allow_events, archive_in, false);
    parseAppsecJSONKey<bool>("drop-events", drop_events, archive_in, false);
}

bool
AppsecTriggerAccessControlLogging::isAllowEvents() const
{
    return allow_events;
}

bool
AppsecTriggerAccessControlLogging::isDropEvents() const
{
    return drop_events;
}

void
AppsecTriggerAdditionalSuspiciousEventsLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec Trigger - Additional Suspicious Events Logging";
    parseAppsecJSONKey<bool>("enabled", enabled, archive_in, true);
    parseAppsecJSONKey<bool>("response-body", response_body, archive_in, false);
    parseAppsecJSONKey<string>("minimum-severity", minimum_severity, archive_in, "high");
}

bool
AppsecTriggerAdditionalSuspiciousEventsLogging::isEnabled() const
{
    return enabled;
}

bool
AppsecTriggerAdditionalSuspiciousEventsLogging::isResponseBody() const
{
    return response_body;
}

const string &
AppsecTriggerAdditionalSuspiciousEventsLogging::getMinimumSeverity() const
{
    return minimum_severity;
}

void
AppsecTriggerLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec Trigger Logging";
    parseAppsecJSONKey<bool>("all-web-requests", all_web_requests, archive_in, false);
    parseAppsecJSONKey<bool>("detect-events", detect_events, archive_in, false);
    parseAppsecJSONKey<bool>("prevent-events", prevent_events, archive_in, true);
}

bool
AppsecTriggerLogging::isAllWebRequests() const
{
    return all_web_requests;
}

bool
AppsecTriggerLogging::isDetectEvents() const
{
    return detect_events;
}

bool
AppsecTriggerLogging::isPreventEvents() const
{
    return prevent_events;
}

void
AppsecTriggerExtendedLogging::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec Trigger Extended Logging";
    parseAppsecJSONKey<bool>("http-headers", http_headers, archive_in, false);
    parseAppsecJSONKey<bool>("request-body", request_body, archive_in, false);
    parseAppsecJSONKey<bool>("url-path", url_path, archive_in, false);
    parseAppsecJSONKey<bool>("url-query", url_query, archive_in, false);
}

bool
AppsecTriggerExtendedLogging::isHttpHeaders() const
{
    return http_headers;
}

bool
AppsecTriggerExtendedLogging::isRequestBody() const
{
    return request_body;
}

bool
AppsecTriggerExtendedLogging::isUrlPath() const
{
    return url_path;
}

bool
AppsecTriggerExtendedLogging::isUrlQuery() const
{
    return url_query;
}

void
LoggingService::load(cereal::JSONInputArchive &archive_in)
{
    parseAppsecJSONKey<string>("address", address, archive_in);
    parseAppsecJSONKey<string>("proto", proto, archive_in);
    parseAppsecJSONKey<int>("port", port, archive_in, 514);
}

const string &
LoggingService::getAddress() const
{
    return address;
}

const string &
LoggingService::getProto() const
{
    return proto;
}

int
LoggingService::getPort() const
{
    return port;
}


void
StdoutLogging::load(cereal::JSONInputArchive &archive_in)
{
    parseAppsecJSONKey<string>("format", format, archive_in, "json");
}

const string &
StdoutLogging::getFormat() const
{
    return format;
}

void
AppsecTriggerLogDestination::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec Trigger LogDestination";
    // TBD: support "file"
    parseAppsecJSONKey<bool>("cloud", cloud, archive_in, false);

    StdoutLogging stdout_log;
    parseAppsecJSONKey<StdoutLogging>("stdout", stdout_log, archive_in);
    agent_local = !(stdout_log.getFormat().empty());
    beautify_logs = stdout_log.getFormat() == "json-formatted";
    parseAppsecJSONKey<LoggingService>("syslog-service", syslog_service, archive_in);
    parseAppsecJSONKey<LoggingService>("cef-service", cef_service, archive_in);
}

int
AppsecTriggerLogDestination::getCefServerUdpPort() const
{
    return getCefServiceData().getPort();
}

int
AppsecTriggerLogDestination::getSyslogServerUdpPort() const
{
    return getSyslogServiceData().getPort();
}

bool
AppsecTriggerLogDestination::isAgentLocal() const
{
    return agent_local;
}

bool
AppsecTriggerLogDestination::shouldBeautifyLogs() const
{
    return beautify_logs;
}

bool
AppsecTriggerLogDestination::getCloud() const
{
    return cloud;
}

bool
AppsecTriggerLogDestination::isCefNeeded() const
{
    return !getCefServiceData().getAddress().empty();
}

bool
AppsecTriggerLogDestination::isSyslogNeeded() const
{
    return !getSyslogServiceData().getAddress().empty();
}

const
string & AppsecTriggerLogDestination::getSyslogServerIpv4Address() const
{
    return getSyslogServiceData().getAddress();
}

const string &
AppsecTriggerLogDestination::getCefServerIpv4Address() const
{
    return getCefServiceData().getAddress();
}

const LoggingService &
AppsecTriggerLogDestination::getSyslogServiceData() const
{
    return syslog_service;
}

const LoggingService &
AppsecTriggerLogDestination::getCefServiceData() const
{
    return cef_service;
}

void
AppsecTriggerSpec::load(cereal::JSONInputArchive &archive_in)
{
    dbgTrace(D_K8S_POLICY) << "Loading AppSec trigger spec";
    parseAppsecJSONKey<AppsecTriggerAccessControlLogging>(
        "access-control-logging",
        access_control_logging,
        archive_in
    );
    parseAppsecJSONKey<AppsecTriggerAdditionalSuspiciousEventsLogging>(
        "additional-suspicious-events-logging",
        additional_suspicious_events_logging,
        archive_in
    );
    parseAppsecJSONKey<AppsecTriggerLogging>("appsec-logging", appsec_logging, archive_in);
    parseAppsecJSONKey<AppsecTriggerExtendedLogging>("extended-logging", extended_logging, archive_in);
    parseAppsecJSONKey<AppsecTriggerLogDestination>("log-destination", log_destination, archive_in);
    parseAppsecJSONKey<string>("name", name, archive_in);
}

const AppsecTriggerAccessControlLogging &
AppsecTriggerSpec::getAppsecTriggerAccessControlLogging() const
{
    return access_control_logging;
}

const string &
AppsecTriggerSpec::getName() const
{
    return name;
}

const AppsecTriggerAdditionalSuspiciousEventsLogging &
AppsecTriggerSpec::getAppsecTriggerAdditionalSuspiciousEventsLogging() const
{
    return additional_suspicious_events_logging;
}

const AppsecTriggerLogging &
AppsecTriggerSpec::getAppsecTriggerLogging() const
{
    return appsec_logging;
}

const AppsecTriggerExtendedLogging &
AppsecTriggerSpec::getAppsecTriggerExtendedLogging() const
{
    return extended_logging;
}

const AppsecTriggerLogDestination &
AppsecTriggerSpec::getAppsecTriggerLogDestination() const
{
    return log_destination;
}

void
TriggersWrapper::save(cereal::JSONOutputArchive &out_ar) const
{
    out_ar(
        cereal::make_nvp("rulebase", triggers_rulebase)
    );
}

// LCOV_EXCL_STOP
