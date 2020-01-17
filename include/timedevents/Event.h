#pragma once

enum Event {
  Event_StatusRequest,
  Event_TailRequest,
  Event_StopRequest,
  Event_PauseRequest,
  Event_ResumeRequest,
  Event_BackupRequest,
  Event_RestoreRequest
};