/*
 *   tasks.h
 *   Copyright (C) 2023 David García Goñi <dagargo@gmail.com>
 *
 *   This file is part of Elektroid.
 *
 *   Elektroid is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Elektroid is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Elektroid. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TASKS_H
#define TASKS_H

#include <gtk/gtk.h>
#include "utils.h"

enum task_list_store_columns
{
  TASK_LIST_STORE_STATUS_FIELD,
  TASK_LIST_STORE_TYPE_FIELD,
  TASK_LIST_STORE_SRC_FIELD,
  TASK_LIST_STORE_DST_FIELD,
  TASK_LIST_STORE_PROGRESS_FIELD,
  TASK_LIST_STORE_STATUS_HUMAN_FIELD,
  TASK_LIST_STORE_TYPE_HUMAN_FIELD,
  TASK_LIST_STORE_REMOTE_FS_ID_FIELD,
  TASK_LIST_STORE_REMOTE_FS_ICON_FIELD,
  TASK_LIST_STORE_BATCH_ID_FIELD,
  TASK_LIST_STORE_MODE_FIELD
};

enum elektroid_task_status
{
  QUEUED,
  RUNNING,
  COMPLETED_OK,
  COMPLETED_ERROR,
  CANCELED
};

enum elektroid_task_mode
{
  ELEKTROID_TASK_OPTION_ASK,
  ELEKTROID_TASK_OPTION_REPLACE,
  ELEKTROID_TASK_OPTION_SKIP
};

enum elektroid_task_type
{
  UPLOAD,
  DOWNLOAD
};

struct task_transfer
{
  struct job_control control;
  gchar *src;			//Contains a path to a file
  gchar *dst;			//Contains a path to a file
  enum elektroid_task_status status;	//Contains the final status
  const struct fs_operations *fs_ops;	//Contains the fs_operations to use in this transfer
  guint mode;
  guint batch_id;
};

struct tasks
{
  struct task_transfer transfer;
  GThread *thread;
  gint batch_id;
  GtkListStore *list_store;
  GtkWidget *tree_view;
  GtkWidget *cancel_task_button;
  GtkWidget *remove_tasks_button;
  GtkWidget *clear_tasks_button;
};

void tasks_init (struct tasks *tasks, GtkBuilder * builder);

gboolean tasks_get_next_queued (struct tasks *tasks, GtkTreeIter * iter,
				enum elektroid_task_type *type, gchar ** src,
				gchar ** dst, gint * fs, guint * batch_id,
				guint * mode);

gboolean tasks_complete_current (gpointer data);

void tasks_cancel_all (GtkWidget * object, gpointer data);

void tasks_visitor_set_batch_canceled (struct tasks *tasks,
				       GtkTreeIter * iter);

void tasks_batch_visitor_set_skip (struct tasks *tasks, GtkTreeIter * iter);

void tasks_batch_visitor_set_replace (struct tasks *tasks,
				      GtkTreeIter * iter);

void tasks_stop_thread (struct tasks *tasks);

const gchar *tasks_get_human_status (enum elektroid_task_status status);

gboolean tasks_check_buttons (gpointer data);

void tasks_visit_pending (struct tasks *tasks,
			  void (*visitor) (struct tasks * tasks,
					   GtkTreeIter * iter));

void tasks_add (struct tasks *tasks, enum elektroid_task_type type,
		const char *src, const char *dst, gint remote_fs_id,
		struct backend *backend);

gboolean tasks_update_current_progress (gpointer data);

#endif
