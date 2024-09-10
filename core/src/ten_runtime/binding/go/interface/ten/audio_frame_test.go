//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//

package ten

import (
	"testing"
	"time"
)

func TestAudioFrame(t *testing.T) {
	frame, err := NewAudioFrame("audio_frame")
	if err != nil {
		t.FailNow()
	}

	if err := frame.AllocBuf(10); err != nil {
		t.FailNow()
	}

	buf, err := frame.LockBuf()
	if err != nil {
		t.FailNow()
	}

	if len(buf) != 10 {
		t.FailNow()
	}

	buf[0] = 1

	if err := frame.UnlockBuf(&buf); err != nil {
		t.FailNow()
	}

	buf, err = frame.LockBuf()
	if err != nil {
		t.FailNow()
	}

	if buf[0] != 1 {
		t.FailNow()
	}

	now := time.Now().UnixMilli()
	if err := frame.SetTimestamp(now); err != nil {
		t.FailNow()
	}

	if timestamp, err := frame.GetTimestamp(); err != nil || timestamp != now {
		t.FailNow()
	}

	if err := frame.SetSampleRate(44100); err != nil {
		t.FailNow()
	}

	if sampleRate, err := frame.GetSampleRate(); err != nil ||
		sampleRate != 44100 {
		t.FailNow()
	}

	if lineSize, err := frame.GetLineSize(); err != nil {
		t.FailNow()
	} else {
		if lineSize != 0 {
			t.FailNow()
		}
	}
}