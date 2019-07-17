; ModuleID = '1.ll'
source_filename = "dana program"

%start = type opaque
%struct_main = type { %start*, i32 }
%struct_hanoi = type { %struct_main*, i32, i8*, i8*, i8* }
%struct_move = type { %struct_hanoi*, i8*, i8* }

@0 = private constant [13 x i8] c"Moving from \00"
@1 = private constant [5 x i8] c" to \00"
@2 = private constant [3 x i8] c".\0A\00"
@3 = private constant [8 x i8] c"Rings: \00"
@4 = private constant [5 x i8] c"left\00"
@5 = private constant [6 x i8] c"right\00"
@6 = private constant [7 x i8] c"middle\00"

declare void @writeInteger(i32)

declare void @writeChar(i8)

declare void @writeByte(i8)

declare void @writeString(i8*)

declare i32 @readInteger()

declare i8 @readChar()

declare i8 @readByte()

declare void @readString(i32, i8*)

declare i32 @extend(i8)

declare i8 @shrink(i32)

declare i32 @strlen(i8*)

declare i32 @strcmp(i8*, i8*)

declare void @strcpy(i8*, i8*)

declare void @strcat(i8*, i8*)

define void @main(%start* %previous) {
entry:
  %new_frame = alloca %struct_main, align 8
  %0 = getelementptr inbounds %struct_main, %struct_main* %new_frame, i64 0, i32 0
  store %start* %previous, %start** %0, align 8
  call void @writeString(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @3, i64 0, i64 0))
  %1 = call i32 @readInteger()
  %2 = getelementptr inbounds %struct_main, %struct_main* %new_frame, i64 0, i32 1
  store i32 %1, i32* %2, align 8
  call void @hanoi(%struct_main* nonnull %new_frame, i32 %1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @4, i64 0, i64 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @5, i64 0, i64 0), i8* getelementptr inbounds ([7 x i8], [7 x i8]* @6, i64 0, i64 0))
  ret void
}

define void @hanoi(%struct_main* %previous, i32 %rings, i8* %source, i8* %target, i8* %auxiliary) {
entry:
  %new_frame = alloca %struct_hanoi
  %0 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 0
  store %struct_main* %previous, %struct_main** %0
  %1 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 1
  store i32 %rings, i32* %1
  %2 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 2
  store i8* %source, i8** %2
  %3 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 3
  store i8* %target, i8** %3
  %4 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 4
  store i8* %auxiliary, i8** %4
  %5 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 1
  %rings1 = load i32, i32* %5
  %greaterorequaltmp = icmp sge i32 %rings1, 1
  br i1 %greaterorequaltmp, label %then, label %endif

return:                                           ; preds = %endif
  ret void

then:                                             ; preds = %entry
  %6 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 0
  %previous2 = load %struct_main*, %struct_main** %6
  %7 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 1
  %rings3 = load i32, i32* %7
  %subtmp = sub i32 %rings3, 1
  %8 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 2
  %9 = load i8*, i8** %8
  %10 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 4
  %11 = load i8*, i8** %10
  %12 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 3
  %13 = load i8*, i8** %12
  call void @hanoi(%struct_main* %previous2, i32 %subtmp, i8* %9, i8* %11, i8* %13)
  %14 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 2
  %15 = load i8*, i8** %14
  %16 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 3
  %17 = load i8*, i8** %16
  call void @move(%struct_hanoi* %new_frame, i8* %15, i8* %17)
  %18 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 0
  %previous4 = load %struct_main*, %struct_main** %18
  %19 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 1
  %rings5 = load i32, i32* %19
  %subtmp6 = sub i32 %rings5, 1
  %20 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 4
  %21 = load i8*, i8** %20
  %22 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 3
  %23 = load i8*, i8** %22
  %24 = getelementptr %struct_hanoi, %struct_hanoi* %new_frame, i32 0, i32 2
  %25 = load i8*, i8** %24
  call void @hanoi(%struct_main* %previous4, i32 %subtmp6, i8* %21, i8* %23, i8* %25)
  br label %endif

endif:                                            ; preds = %then, %entry
  br label %return
}

define void @move(%struct_hanoi* %previous, i8* %source, i8* %target) {
entry:
  %new_frame = alloca %struct_move
  %0 = getelementptr %struct_move, %struct_move* %new_frame, i32 0, i32 0
  store %struct_hanoi* %previous, %struct_hanoi** %0
  %1 = getelementptr %struct_move, %struct_move* %new_frame, i32 0, i32 1
  store i8* %source, i8** %1
  %2 = getelementptr %struct_move, %struct_move* %new_frame, i32 0, i32 2
  store i8* %target, i8** %2
  call void @writeString(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0))
  %3 = getelementptr %struct_move, %struct_move* %new_frame, i32 0, i32 1
  %4 = load i8*, i8** %3
  call void @writeString(i8* %4)
  call void @writeString(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @1, i32 0, i32 0))
  %5 = getelementptr %struct_move, %struct_move* %new_frame, i32 0, i32 2
  %6 = load i8*, i8** %5
  call void @writeString(i8* %6)
  call void @writeString(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @2, i32 0, i32 0))
  br label %return

return:                                           ; preds = %entry
  ret void
}
