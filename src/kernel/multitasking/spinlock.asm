[bits 32]
[section .text]

; ARGS pointer to lock
[global acquire_spinlock]
acquire_spinlock:
  push ebp
  mov ebp, esp

  lock bts word [ebp + 8], 0
  jnc .acquired

  ; Function
.retry:
  pause

  bt word [ebp + 8], 0
  jc .retry

  lock bts word [ebp + 8], 0
  jc .retry

.acquired:


  mov esp, ebp
  pop ebp
  ret


; ARGS pointer to lock
; RET 0 on success 1 on failure
[global release_spinlock]
release_spinlock:
  push ebp
  mov ebp, esp

  ; Function
  lock btr word [ebp + 8], 0
  jc .wasLocked
  jnc .wasntLocked
.wasLocked:
  mov eax, 0
  jmp .return

.wasntLocked:
  mov eax, 1
  jmp .return

.return:

  mov esp, ebp
  pop ebp
  ret
